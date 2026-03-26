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
#include "ps2/sound.h"
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
#include "syscall/syscall.h"
#include "ps2/mouse.h"
#include "uacpi/uacpi.h"
#include "uacpi/event.h"
#include "uacpi/sleep.h" // to tell the system to shut the fuck down
#include "identity.h"
#include "vmm.h"
#include "elf.h"



#define FILE_NAME "Kernel Entry (/main.c)"
#define CODE_QUALITY "A"
#define FILE_VERSION "2.0"
#define FILE_AUTHOR "Afif Ali Saadman(afifafifafifafifali)"
#define FILE_DESCRIPTION "Kernel Entry Point,Limine Requests, Important memcpy,memmove functions, Drivers initiation are done here."
#define FILE_LAST_UPDATED_DATE "2/3/2026"

/* ===================================================== */
/* ================= LIMINE SETUP ====================== */
/* ===================================================== */


extern void cmd_cp(const char* src, const char* dst);
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

void *memmove(void *dstptr, const void *srcptr, size_t size) {
  unsigned char       *dst = (unsigned char *)dstptr;
  const unsigned char *src = (const unsigned char *)srcptr;
  if (dst < src) {
    for (size_t i = 0; i < size; i++)
      dst[i] = src[i];
  } else {
    for (size_t i = size; i != 0; i--)
      dst[i - 1] = src[i - 1];
  }
  return dstptr;
}

void test_sbrk() {
    serial_io_printf("Testing _sbrk\n");

    void* p1 = _sbrk(1024);  // allocate 1 KiB
    void* p2 = _sbrk(2048);  // allocate 2 KiB
    void* p3 = _sbrk(0);     // current break

    serial_io_printf("p1 = %p\n", p1);
    serial_io_printf("p2 = %p\n", p2);
    serial_io_printf("current break = %p\n", p3);
}

void test_brk() {
    serial_io_printf("Testing brk\n");

    void* current = _sbrk(0);           // get current break
    serial_io_printf("Current break = %p\n", current);

    void* new_addr = (uint8_t*)current + 4096; // move 4 KiB
    if (brk(new_addr) == 0) {
        serial_io_printf("brk success, new break = %p\n", _sbrk(0));
    } else {
        serial_io_printf("brk failed\n");
    }
}


/* ===================================================== */
/* ================== CPU HELPERS ====================== */
/* ===================================================== */



static inline uint64_t read_cr0(void) {
    uint64_t v;
    asm volatile("mov %%cr0,%0" : "=r"(v));
    return v;
}

static inline uint64_t read_cr4(void) {
    uint64_t v;
    asm volatile("mov %%cr4,%0" : "=r"(v));
    return v;
}

static inline void write_cr0(uint64_t v) {
    asm volatile("mov %0,%%cr0" :: "r"(v));
}

static inline void write_cr4(uint64_t v) {
    asm volatile("mov %0,%%cr4" :: "r"(v));
}


void shutthefuckdown(void){
    uacpi_status ret = uacpi_prepare_for_sleep_state(UACPI_SLEEP_STATE_S5);
    if (uacpi_unlikely_error(ret)) {
        serial_io_printf("failed to prepare for sleep: %s\n", uacpi_status_to_string(ret));
        
    }
    asm("cli");
    ret = uacpi_enter_sleep_state(UACPI_SLEEP_STATE_S5);
    if (uacpi_unlikely_error(ret)) {
        serial_io_printf("failed to enter sleep: %s\n", uacpi_status_to_string(ret));
        
    }
    serial_io_printf("Should never reach here\n");
}
/* ===================================================== */
/* ==================== SSE ============================ */
/* ===================================================== */

static void enable_sse(void) {

    uint64_t cr0 = read_cr0();
    uint64_t cr4 = read_cr4();

    /* Clear EM, set MP */
    cr0 &= ~(1 << 2);
    cr0 |=  (1 << 1);

    write_cr0(cr0);

    /* Enable OSFXSR + OSXMMEXCPT */
    cr4 |= (1 << 9) | (1 << 10);

    write_cr4(cr4);
}

/* ===================================================== */
/* ================== HALT ============================= */
/* ===================================================== */

__attribute__((noreturn))
static void hcf(void) {
    for (;;) asm volatile("hlt");
}

  #include "PhysAlloc.h"

/* ===================================================== */
/* ================== MAIN ============================= */
/* ===================================================== */


void paging_test_run(void) {
    serial_io_printf("\n=== Paging unmap test ===\n");

    // Pick some random virtual address nobody else is using
    void* test_virt = (void*)0xFFFF900000000000;
    void* test_phys = (void*)0x0000000000100000; // some known free phys page

    // Map it
    mapPage(test_virt, test_phys, 0x03); // present + writable
    serial_io_printf("Mapped test page\n");

    // Write to it
    volatile uint64_t *ptr = (volatile uint64_t *)test_virt;
    *ptr = 0xDEADBEEFCAFEBABE;
    serial_io_printf("Wrote 0xDEADBEEFCAFEBABE\n");

    // Read back
    uint64_t val = *ptr;
   serial_io_printf("Read back: 0x%lx  ", val);
serial_io_printf("%s\n", val == 0xDEADBEEFCAFEBABE ? "[PASS]" : "[FAIL]");

    // Unmap it
    unmapPage(test_virt);
    serial_io_printf("Unmapped test page\n");

    // This should page fault (Vector 14)
    serial_io_printf("About to access unmapped page - expect #PF...\n");
    val = *ptr;

    // Should never reach here
    serial_io_printf("ERROR: should have page faulted! [FAIL]\n");
}

void kmain(void) {

    /* ========== Early CPU ========== */

    enable_sse();

    if (!LIMINE_BASE_REVISION_SUPPORTED)
        hcf();

    /* ========== Framebuffer ========== */

    if (!framebuffer_request.response ||
        framebuffer_request.response->framebuffer_count < 1)
        hcf();

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    initialize_terminal(fb);

    serial_io_init();

    printf("DoorsOS Booting...\n");
    serial_io_printf("Serial Online\n");

    /* ========== Memory ========== */

    if (!memmap_request.response ||
        !hhdm_request.response)
        hcf();

    setMemoryMap(4);
    return_cpu();
    phys_alloc_init();
    allocator_init();
    vmm_init();
    phys_alloc_test();
    vmm_test();
    initPML4();

    
    printf("Memory OK\n");

    /* ========== GDT / IDT / PIC ========== */

    asm volatile("cli");

    initiateGDT();

    remap_pic(0x20, 0x28);

    
    mouse_install();
    ps2_kbio_init();
    pit_init(100);
    init_idt();


    asm volatile("sti");

    printf("Interrupts Ready\n");
    
  
    void *gay = k_malloc(6969);
    timer_sleep_ms(3100);
    serial_io_printf("Ticks: %llu \n",timer_get_ticks());
        

  
    
    uacpi_status ret = uacpi_initialize(0);

    if (uacpi_unlikely_error(ret)) {
        serial_io_printf("\n\n yea,i am jobless \n\n");
    }

     ret = uacpi_namespace_load();
    if (uacpi_unlikely_error(ret)) {
        serial_io_printf("uacpi_namespace_load error: %s", uacpi_status_to_string(ret));
        
    }

    ret = uacpi_namespace_initialize();
    if (uacpi_unlikely_error(ret)) {
        serial_io_printf("uacpi_namespace_initialize error: %s", uacpi_status_to_string(ret));
    }

    ret = uacpi_finalize_gpe_initialization();

    if (uacpi_unlikely_error(ret)) {
        serial_io_printf("uacpi_gpe_gay error: %s", uacpi_status_to_string(ret));
    }


    uacpi_status ret2 = uacpi_prepare_for_sleep_state(UACPI_SLEEP_STATE_S5);
    if (uacpi_unlikely_error(ret2)) {
        serial_io_printf("failed to prepare for sleep: %s\n", uacpi_status_to_string(ret2));
        
    }
   
    

    serial_io_printf("PS/2 OK\n");

    printMemoryMaps();
   
    
    rtl8139_init();

    storage_init();
    lspci();
    kprint("\e[2J\e[H");
    if (storage_get_type() != STORAGE_NONE) {

        printf("Mounting FAT32...\n");

        if (fat32_mount(2048, false))
            printf("FAT32 Mounted\n");
        else
            printf("FAT32 Failed\n");
    }

    syscall_init();

    // Test syscalls
    printf("\n=== Testing int 0x80 Syscalls ===\n");
    const char* test_msg = "Hello from int 0x80 syscall!\n";
    uint64_t written = sys_print_write(1, test_msg, strlen(test_msg));
    printf("sys_write returned: %ld\n", written);

    printf("=== Syscall Test Complete YAY ===\n\n");


    //initTasking();


    printf("Scheduler Ready\n");


    printf("\n=== DoorsOS Ready ===\n");
    sound_init();
    
        // Do Re Mi Fa So La Ti Do
    sound_play(261, 400); // do
    timer_sleep_ms(100);
    speaker_off();
    sound_play(294, 400); // re
    timer_sleep_ms(100);
    speaker_off();
    sound_play(329, 400); // mi
    timer_sleep_ms(100);
    speaker_off();
    sound_play(349, 400); // fa
    timer_sleep_ms(100);
    speaker_off();
    sound_play(392, 400); // so
    timer_sleep_ms(100);
    speaker_off();
    sound_play(440, 400); // la
    timer_sleep_ms(100);
    speaker_off();
    sound_play(493, 400); // ti
    timer_sleep_ms(100);
    speaker_off();
    sound_play(523, 600); // do high
    timer_sleep_ms(200);
    speaker_off();
    
     printf("\nDoorsOS Shell v2.0\nCopyright(c),Afif Ali Saadman, 2025 or whatever year it is\n");
    printf("Type 'help' for commands\n\n");

    /* ========== ELF Loader Test ========== */
    printf("\n=== ELF Loader Test ===\n");

    elf64_program_t test_prog;
    elf_error_t err = elf64_load_file("/test_add", &test_prog);

    if (err == ELF_OK) {
        serial_io_printf("ELF loaded successfully!\n");
        serial_io_printf("Entry point: 0x%lx\n", test_prog.entry);
        serial_io_printf("Base address: 0x%lx\n", test_prog.base);
        serial_io_printf("Size: %lu bytes\n", test_prog.size);

        /* Allocate a stack for the ELF program (8KB) */
        uint8_t *elf_stack = (uint8_t *)vmm_alloc_pages(2);
        if (!elf_stack) {
            serial_io_printf("Failed to allocate ELF stack!\n");
        } else {
            uint64_t stack_top = (uint64_t)elf_stack + 0x2000;
            serial_io_printf("ELF stack allocated at: 0x%lx (top: 0x%lx)\n",
                             (uint64_t)elf_stack, stack_top);

            /* Call the  fucking entry point with its own stack */
            serial_io_printf("Running ELF binary with syscall...\n");

          uint64_t result;
            char *argv[] = { "program_name", "arg1", "arg2", NULL };
            char *envp[] = {"HOME=/","KERNEL=/efi/boot","basharbai","nawfle","laden","obama",NULL};
            uint64_t argc = 3;

            asm volatile(
                "mov %%rsp, %%r12\n\t"         // save old stack
                "mov %4, %%rsp\n\t"            // switch to ELF stack
                "mov %1, %%rdi\n\t"            // argc
                "mov %2, %%rsi\n\t"            // argv
                "mov %3, %%rdx\n\t"            // envp
                "call *%5\n\t"                 // call ELF entry
                "mov %%r12, %%rsp\n\t"         // restore old stack
                : "=a"(result)
                : "r"(argc), "r"(argv), "r"(envp), "r"(stack_top), "r"(test_prog.entry)
                : "r12", "rdi", "rsi", "rdx", "memory"
            );
            serial_io_printf("ELF binary returned: %lu\n", result);
            serial_io_printf("=== ELF Syscall Test Complete ===\n");
           elf64_unload(&test_prog);
        }
    } else {
        serial_io_printf("ELF load failed: %s\n", elf64_strerror(err));
    }

    printf("=== ELF Test Complete ===\n\n");
    /* ================================= */

    #include "interrupts/pipe.h"
    serial_io_printf("=== PIPE TEST START ===\n");

    pipe_t* p = pipe_create(64);
    if (!p) {
        serial_io_printf("pipe_create FAILED\n");
        return;
    }

    serial_io_printf("pipe_create OK\n");

    char msg[] = "hello from pipe!";
    int wrotten = pipe_write(p, (uint8_t*)msg, sizeof(msg));

    serial_io_printf("Written bytes: %d\n", wrotten);

    char buf[64];
    memset(buf, 0, sizeof(buf));

    int read = pipe_read(p, (uint8_t*)buf, sizeof(buf));

    serial_io_printf("Read bytes: %d\n", read);
    serial_io_printf("Data read: %s\n", buf);

    // Edge case: read from empty pipe
    int read_empty = pipe_read(p, (uint8_t*)buf, sizeof(buf));
    serial_io_printf("Read from empty pipe: %d (expected 0)\n", read_empty);

    // Fill pipe fully
    char big[128];
    for (int i = 0; i < 128; i++) big[i] = 'A';

    int big_write = pipe_write(p, (uint8_t*)big, 128);
    serial_io_printf("Write beyond capacity: %d (should <= 64)\n", big_write);

    // Close write end and try writing
    pipe_close_write(p);
    int write_closed = pipe_write(p, (uint8_t*)"x", 1);
    serial_io_printf("Write after close: %d (expected -1)\n", write_closed);

    // Close read end and try reading
    pipe_close_read(p);
    int read_closed = pipe_read(p, (uint8_t*)buf, 1);
    serial_io_printf("Read after close: %d (expected -1)\n", read_closed);

    pipe_destroy(p);

    serial_io_printf("=== PIPE TEST END ===\n");
    fd_test_complete();

    serial_io_printf("=====Beginning le so test====\n");
    so_test_run();

    serial_io_printf("\n\n");
    test_sbrk();
    test_brk();

    
 test_mmap_basic();

    cmd_cp("/govdb.csv","/nigga.csv");
  
    initTasking();
    
}