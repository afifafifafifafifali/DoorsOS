#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "gfx/term.h"
#include "flanterm/src/flanterm.h"
#include "flanterm/src/flanterm_backends/fb.h"
#include "interrupts/isr.h"
#include "fs/ahci_driver.h"
#include "datandtime.h"
#include "info/cpuinfo.h"
#include "fadt_head.h"
#include "interrupts/timer.h"
#include "uacpi/kernel_api.h"
#include "math.h"
#include "uacpi/tables.h"
#include "uacpi/acpi.h"
#include "interrupts/pic.h"
#include "bootloader.h"
#include "mem/new/pmm.h"
#include "interrupts/idt.h"
#include "fs/pci.h"
#include "info/meminfo.h"
#include "gfx/printf.h"
#include "gfx/serial_io.h"
#include "fs/fat32.h"
#include "libs/string.h"
#include "ps2/kbio.h"
#include "gdt.h"
#include "mem/paging.h"
#include "fs/ata.h"
#include "mem/heap.h"
#include "shell/shell_enhanced.h"
#include "storage/storage.h"
#include "rtl8139/rtl8139.h"
#include "uacpi/uacpi.h"
#include <limine.h>
#include "tasks/task.h"


struct acpi_sdt_header {
    char     signature[4];
    uint32_t length;
    uint8_t  revision;
    uint8_t  checksum;
    char     oem_id[6];
    char     oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
};

// Limine base revision = 3
__attribute__((used, section(".limine_requests")))
volatile LIMINE_BASE_REVISION(3);

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;
    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }
    return 0;
}
/*
static inline void enable_sse(void) {
    uint64_t cr0, cr4;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~(1 << 2);
    cr0 |=  (1 << 1);
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 9) | (1 << 10);
    __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));
}
*/


static inline uint64_t read_cr4(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr4, %0" : "=r"(val));
    return val;
}
static inline void write_cr0(uint64_t val) {
    __asm__ volatile("mov %0, %%cr0" :: "r"(val));
}

static inline void write_cr4(uint64_t val) {
    __asm__ volatile("mov %0, %%cr4" :: "r"(val));
}

static inline uint64_t read_cr0(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr0, %0" : "=r"(val));
    return val;
}

static inline void enable_sse(void) {
    uint64_t cr0 = read_cr0();
    uint64_t cr4 = read_cr4();

    cr0 &= ~(1UL << 2);  
    cr0 |=  (1UL << 1);   
    write_cr0(cr0);

    cr4 |= (1UL << 9) |   
           (1UL << 10);    

    
    uint32_t eax, ebx, ecx, edx;
    __asm__ volatile(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1)
    );
    if (ecx & (1 << 26))   
        cr4 |= (1UL << 18); 

    write_cr4(cr4);

    uint32_t xcr0_low = 0, xcr0_high = 0;
    if (cr4 & (1UL << 18)) {
        xcr0_low  = 0x3;
        xcr0_high = 0x0;
        __asm__ volatile("xsetbv" :: "c"(0), "a"(xcr0_low), "d"(xcr0_high));
    }
}
static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

static bool aml_find_s5(uint8_t *aml, uint32_t len,
                        uint8_t *s5a, uint8_t *s5b)
{
    for (uint32_t i = 0; i + 8 < len; i++) {
        if (aml[i] == '_' &&
            aml[i+1] == 'S' &&
            aml[i+2] == '5' &&
            aml[i+3] == '_') {

            uint8_t *p = &aml[i+4];

            // Skip NameOp if present
            if (*p == 0x08) p++;

            // Must be PackageOp
            if (*p != 0x12) continue;
            p++;

            // Skip package length byte(s)
            if (*p & 0x80)
                p += (*p & 0x3F);
            else
                p++;

            // Must be 2-element package
            if (*p != 0x02) continue;
            p++;

            // First integer
            if (*p == 0x0A) p++;  // BytePrefix
            *s5a = *p++;

            // Second integer
            if (*p == 0x0A) p++;
            *s5b = *p++;

            return true;
        }
    }
    return false;
}

// Test task 1
void test_task_1(void) {
    while(1) {
        serial_io_printf("Task 1 running...\n");
        task_yield(); // Yield to other tasks
    }
}

// Test task 2
void test_task_2(void) {
    while(1) {
        serial_io_printf("Task 2 running...\n");
        task_yield(); // Yield to other tasks
    }
}

void kmain(void) {
    // ---------------Who ever types code in this area is gay except sse---------------------------
    enable_sse();
    // -----------------------Code end--------------------------------------------------

    //------------------------Float enabled here nigga---------------------------------
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }
    
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }
    
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    initialize_terminal(framebuffer);

    serial_io_printf("%f",sqrt(9));
    
    printf("DoorsOS Kernel Booted!\n");
    
    if (memmap_request.response == NULL || hhdm_request.response == NULL) {
        printf("Limine memmap or HHDM response is NULL. Halting.\n");
        hcf();
    }
    
    setMemoryMap(4);
    allocator_init();

    initPML4();

    __asm__ volatile ("cli");
    initiateGDT();
    remap_pic(0x20, 0x28);
    init_idt();
    //timer_init(600000000ULL);
    

    //timer_sleep_ms(3000);

    // Initialize ACPI after paging is set up but before enabling interrupts
    //init_acpi();  // Commented out to prevent page fault



    serial_io_printf("INT START\n");
    enable_interrupts();
    serial_io_printf("INT end\n");
    serial_io_printf("initing pit \n");
    pit_init(100);
    serial_io_printf("pit test \n");
    pit_test();
    serial_io_printf("Measure freq\n");
    clk_speed = measure_cpu_frequency_with_pit();
    serial_io_printf("%llu \n",clk_speed);
  return_cpu();
    //serial_io_printf(" %s \n %llu \n", vendor, memory_amount);  //PF happens here

    static uint8_t table_buffer[4096];

    uacpi_status status =
        uacpi_setup_early_table_access(
            table_buffer, sizeof(table_buffer)
        );

    if (status != UACPI_STATUS_OK) {
        serial_io_printf("uACPI early setup failed: %d\n", status);
    }

    uacpi_phys_addr rsdp_phys;
    status = uacpi_kernel_get_rsdp(&rsdp_phys);

    if (status != UACPI_STATUS_OK) {
        serial_io_printf("RSDP not found!\n");
    }

    /* Map ACPI 1.0 size first */
    struct acpi_rsdp *rsdp =
        (struct acpi_rsdp *)uacpi_kernel_map(rsdp_phys, 20);

    if (!rsdp) {
        serial_io_printf("Failed to map RSDP\n");
    }

    /* ACPI 2.0+ → remap full table */
    if (rsdp->revision >= 2) {
        rsdp = (struct acpi_rsdp *)uacpi_kernel_map(
            rsdp_phys, rsdp->length
        );

        if (!rsdp) {
            serial_io_printf("Failed to remap full RSDP\n");
        }
    }

    serial_io_printf("ACPI revision: %u\n", rsdp->revision);
  
struct acpi_fadt *fadt;
uacpi_status st = uacpi_table_find_by_signature(
    ACPI_FADT_SIGNATURE,
    (struct acpi_sdt_header **)&fadt
);

serial_io_printf("FADT revision: %u, PM1a_CNT: 0x%x\n",
                 fadt->hdr.revision,
                 fadt->pm1a_cnt_blk);


    ps2_kbio_init();
    serial_io_printf("DEBUG: Before rtl8139_init\n");
    rtl8139_init();
    
    printf("=== DoorsOS System Initialized ===\n");
    serial_io_printf("=== DoorsOS System Initialized ===\n");
    
    // auto-detect storage (checks boot sector)
    storage_init();

    
    printf("Storage init done!\n");
    serial_io_printf("DEBUG: After storage_init\n");
    
    if (storage_get_type() != STORAGE_NONE) {
        printf("Mounting FAT32...\n");
        bool mounted = fat32_mount(2048, false);
        printf("FAT32 mount: %s\n", mounted ? "OK" : "FAILED");
    }
    
    serial_io_printf("DEBUG: Before tasking init\n");


    serial_io_printf("DEBUG: Before shell\n");

    printf("\n");
    serial_io_printf("DEBUG: Starting shell\n");

    initTasking();

    //shell_run();
    //outw(pm1a_cnt_blk, (1 << 13) | (s4bios_req ? (1 << 10) : 0)); // Why tf this shit works in kernel..
    hcf();
}
