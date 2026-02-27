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




/* ===================================================== */
/* ================= LIMINE SETUP ====================== */
/* ===================================================== */

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

/* ===================================================== */
/* ================== MAIN ============================= */
/* ===================================================== */

void kmain(void) {

    /* ========== Early CPU ========== */

    enable_sse();

    if (!LIMINE_BASE_REVISION_SUPPORTED)
        hcf();

    /* ========== Framebuffer ========== */

    if (!framebuffer_request.response ||
        framebuffer_request.response->framebuffer_count < 1)
        hcf();

    struct limine_framebuffer *fb =
        framebuffer_request.response->framebuffers[0];

    initialize_terminal(fb);

    serial_io_init();

    printf("DoorsOS Booting...\n");
    serial_io_printf("Serial Online\n");

    /* ========== Memory ========== */

    if (!memmap_request.response ||
        !hhdm_request.response)
        hcf();

    setMemoryMap(4);
    allocator_init();
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

    timer_sleep_ms(3100);
    serial_io_printf("Ticks: %llu \n",timer_get_ticks());
    
    
     allocator_init();
     
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
    if (storage_get_type() != STORAGE_NONE) {

        printf("Mounting FAT32...\n");

        if (fat32_mount(2048, false))
            printf("FAT32 Mounted\n");
        else
            printf("FAT32 Failed\n");
    }

    syscall_init();

  

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

    initTasking();
    for(;;){}
}