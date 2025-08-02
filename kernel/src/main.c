#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "gfx/term.h"
#include "flanterm/src/flanterm.h"
#include "flanterm/src/flanterm_backends/fb.h"
#include "interrupts/isr.h"
#include "fs/ahci.h"
#include "interrupts/multitasking.h"
#include "snake.h"
#include "info/cpuinfo.h"
#include "interrupts/timer.h"
#include "fs/detect_ahci.h"
#include "rtl8139/rtl8139.h"
#include "bootloader.h"
#include "sghsc_logo.h"
#include "mem/new/pmm.h"
#include "gui/colorama.h"
#include "gui/windows.h"
#include "interrupts/idt.h"
#include "fs/pci.h"
#include "fs/ahci.h"
#include "info/meminfo.h"
#include "gfx/printf.h"
#include "gfx/serial_io.h"
#include "fs/fat32.h"
#include "libs/string.h"
#include "ps2/mouse.h"
#include "ps2/kbio.h"
#include "gdt.h"
#include "mem/paging.h"
#include "gfx/printf.h"
#include "fs/ata.h"

#include "mem/paging.h"
#include "mem/heap.h"
#include <limine.h>

// Limine base revision = 3

__attribute__((used, section(".limine_requests")))
volatile LIMINE_BASE_REVISION(3);


void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
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

// Enable SSE
static inline void enable_sse(void) {
    uint64_t cr0, cr4;

    // Read CR0
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~(1 << 2);  // Clear EM (bit 2) to disable FPU emulation
    cr0 |=  (1 << 1);  // Set MP (bit 1) to monitor FPU

    // Write back CR0
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));

    // Read CR4
    __asm__ volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 9) | (1 << 10);  // Set OSFXSR (bit 9) and OSXMMEXCPT (bit 10)

    // Write back CR4
    __asm__ volatile("mov %0, %%cr4" :: "r"(cr4));
}

// Halt and catch fire function.
static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

// Print doors ASCII art
void print_doors_logo() {
    printf("________                             ________    _________\n");
    printf("\\______ \\   ____   ___________  _____\\_____  \\  /   _____/\n");
    printf(" |    |  \\ /  _ \\ /  _ \\_  __ \\/  ___//   |   \\ \\_____  \\ \n");
    printf(" |    `   (  <_> |  <_> )  | \\/\\___ \\/    |    \\/        \\\n");
    printf("/_______  /\\____/ \\____/|__|  /____  >_______  /_______  /\n");
    printf("        \\/                         \\/        \\/        \\/ \n");
}
#define COLOR_BLACK 0x000000
void clear_screen(void) {
    struct limine_framebuffer* fb = framebuffer_request.response->framebuffers[0];
    uint64_t pixels = fb->width * fb->height;
    uint32_t* framebuffer = (uint32_t*) fb->address;

    for (uint64_t i = 0; i < pixels; i++) {
        framebuffer[i] = COLOR_BLACK;
    }
    // Move cursor to top-left
    kprint("\e[2J\e[H");

}

#include <stdint.h>

// Define Bangladesh flag colors
#define COLOR_BD_GREEN  0x006A4E
#define COLOR_BD_RED    0xF42A41

// Draw Bangladesh flag at (x, y) with width w and height h
void draw_bangladesh_flag(int x, int y, int w, int h) {
    // Draw green rectangle background
    draw_rect(x, y, w, h, COLOR_BD_GREEN);

    // Circle radius: about 3/10 height
    int radius = (3 * h) / 10;

    // Circle center x: about 9/20 width from left side
    int cx = x + (9 * w) / 20;

    // Circle center y: middle vertically
    int cy = y + h / 2;

    // Draw red circle
    draw_circle(cx, cy, radius, COLOR_BD_RED);
}


void draw_sghsc_logo_exact(int x, int y) {
    for (int j = 0; j < 64; j++) {
        for (int i = 0; i < 64; i++) {
            uint32_t color = sghsc_logo[j][i];
            putPixel(x + i, y + j, color);
        }
    }
}


void minimal_bash() {
    bool ntcp = false;
    while (1) {
        if (ntcp) {
            printf("\n\n\n\n");
            ntcp = false;
        }
        kprint("root@testpc# ");

        string_t input = (string_t)malloc(2024);
        if (!input) {
            printf("Memory allocation failed for input buffer.\n");
            return;
        }

        string_t result = ps2_kbio_read(input, 2024);
        if (result != NULL) {
            printf("\n");
            // Basic commands
            if (strEql(result, "ata_identify")) {
                identify();
            } else if (strEql(result, "neofetch")) {
                print_doors_logo();
                printf("\n");
                kprint_color("Welcome to DoorsOS", COLOR_RGB_GREEN, true, COLOR_BLACK, false);
                printf("\nThis has true color enabled with RGB support, run sghsc to see it's proof.\nYou can find the source code at \"https://github.com/afifafifafifafifali/DoorsOS\"\n");
            } else if (strEql(result, "clear")) {
                clear_screen();
            } else if (strEql(result, "paging")) {
                check_paging();
            } else if (strEql(result, "sse_test")) {
                test_sse();
            } else if (strEql(result, "exit")) {
                printf("Exiting...\n");
                free(input);
                free(result);
                return;
            }
            // echo
            else if (strncmp(result, "echo ", 5) == 0) {
                printf("%s\n", result + 5);
            }
            // ls
            else if (strncmp(result, "ls ", 3) == 0) {
                string_t pathy = result + 3;
                if (strEql(pathy, "/")) {
                    fat32_list_root();
                } else if (strEql(pathy, " ")) {
                    if (strEql(current_directory, "/")) {
                        fat32_list_root();
                    } else {
                        ls(NULL);
                    }
                } else {
                    ls(pathy);
                }
            }
            // cat
            else if (strncmp(result, "cat ", 4) == 0) {
                string_t filename = result + 4;
                if (strEql(filename, " ")) {
                    printf("Missing 1 required argument: filename\n");
                } else {
                    cat(filename);
                }
            }
            else if (strEql("A", result)) {
                // RGB: red foreground on 4-bit blue background
                kprint_color("Hello", 0xFF0000, true, 4, false);
                kprint_color("World", 0xFFFFFF, true, 0x202020, true);
                kprint_color("Classic ANSI", 11, false, 9, false);
                kprint_color_at(10, 5, "Hello", 0xFF0000, true, 0x000000, true);
            }
            else if (strEql("rect", result)) {
                clear_screen();
                draw_rect(10, 10, 100, 50, COLOR_RGB_PINK);
            }
            else if (strEql("circ", result)) {
                clear_screen();
                // Draw a blue filled circle at (100, 100) with radius 50
                draw_circle(100, 100, 50, COLOR_RGB_RED);
            }
            else if (strEql("bd", result)) {
                clear_screen();
                int flag_x = 50;
                int flag_y = 50;
                int flag_w = 300;
                int flag_h = 200;
                int text_x = flag_x - 30;
                int text_y = flag_y - 30;
                uint32_t fg = COLOR_RGB_RED;
                uint32_t bg = COLOR_RGB_GREEN;
                kprint_color_at(text_x, text_y, "DoorsOS made in Bangladesh by Afif Ali Saadman.", fg, true, bg, true);
                printf("Printing my school logo and the Bangladeshi national flag\n");
                draw_bangladesh_flag(flag_x, flag_y, flag_w, flag_h);
            }
            else if (strEql("sghsc", result)) {
                clear_screen();
                kprint_color("\n\nThis Operating System (DoorsOS) was made by Afif Ali Saadman, And is under the MIT license. \nMade with love from Bangladesh\n", COLOR_RGB_CYAN, true, COLOR_RGB_BLACK, true);
                printf("[bitmap_array_framebuffer]Printing my school logo and the Bangladeshi national flag.\n\n\n");
                draw_sghsc_logo_exact(80, 80);
                draw_bangladesh_flag(150, 80, 70, 80);
                ntcp = true;
            }
            // mkfile
            else if (strncmp(result, "mkfile ", 7) == 0) {
                string_t filename = result + 7;
                printf("Enter contents for %s (type ':end' alone on a line to finish):\n", filename);

                size_t max_content_size = 16384;
                string_t content = (string_t)malloc(max_content_size);
                if (!content) {
                    printf("Memory allocation failed.\n");
                    free(input);
                    free(result);
                    break;
                }
                memset(content, 0, max_content_size);

                size_t content_len = 0;
                while (1) {
                    printf("> ");
                    char line[1024];
                    string_t entered = ps2_kbio_read(line, 1023);
                    if (!entered) {
                        printf("Input error.\n");
                        break;
                    }

                    if (strcmp(entered, ":end") == 0) {
                        free(entered);
                        break;
                    }

                    size_t line_len = strlen(entered);
                    if (content_len + line_len + 1 >= max_content_size) {
                        printf("File content too long, stopping input.\n");
                        free(entered);
                        break;
                    }

                    memcpy(content + content_len, entered, line_len);
                    content_len += line_len;
                    content[content_len++] = '\n';
                    content[content_len] = '\0';

                    free(entered);
                }

                printf("\nCreating file '%s'...\n", filename);
                if (fat32_write_file(filename, (const uint8_t *)content, content_len)) {
                    printf("File '%s' created successfully.\n", filename);
                } else {
                    printf("Failed to create file '%s'.\n", filename);
                }

                free(content);
                free(input);
                free(result);
            }
            else if(strEql("snake",result)){
                clear_screen();
                snake_run();
                clear_screen();
            }
            else if (strEql(result, "")) {
                // Do nothing for empty input
            }
            // unknown
            else {
                printf("Invalid Command: %s\n", result);
            }

            free(input);
            free(result);
        } else {
            kprint("Failed to read input!\n");
            free(input);
        }
 
    }
}
// Read cr0 function
static inline uint64_t read_cr0(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr0, %0" : "=r"(val));
    return val;
}

// Check paging
void check_paging() {
    uint64_t cr0 = read_cr0();
    if (cr0 & (1ULL << 31)) {
        kprint("Paging is enabled.\n");
    } else {
        kprint("Paging is NOT enabled.\n");
    }
}

static void itoa(unsigned int value, char* str) {
    char buffer[10];
    int i = 0;
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }
    int j = 0;
    while (i > 0) {
        str[j++] = buffer[--i];
    }
    str[j] = '\0';
}
// Test SSE function
void test_sse() {
    float input = 21.0f;
    float output;

    __asm__ volatile (
        "movss %1, %%xmm0\n\t"
        "addss %%xmm0, %%xmm0\n\t"
        "movss %%xmm0, %0\n\t"
        : "=m"(output)
        : "m"(input)
        : "xmm0"
    );

    // Convert float to int by truncation
    unsigned int int_output = (unsigned int)output;

    char buf[20];
    kprint("SSE test: 21 * 2 = ");
    itoa(int_output, buf);
    kprint(buf);
    kprint("\n");
}

// Test page mapping
void test_page_mapping() {
    serial_io_printf("HHDM variable is being setted\n");
    uintptr_t hhdm = hhdm_request.response->offset;

    // Allocate a physical page (virtual address returned)
    serial_io_printf("Physical allocating phys_page\n");
    uintptr_t phys_page_virtual = k_malloc(4096);
    if (!phys_page_virtual) {
        printf("Failed to allocate physical page!\n");
        return;
    }

    serial_io_printf("phys_page (virtual) = %p\n", (void*)phys_page_virtual);
    serial_io_printf("HHDM offset: %p\n", (void*)hhdm);

    // Zero out memory using virtual address
    memset((void*)phys_page_virtual, 0, 4096);

    // Convert virtual to physical by subtracting HHDM base
    uintptr_t phys_page = phys_page_virtual - hhdm;

    serial_io_printf("Mapping virtual address to physical page\n");
    printf("Now, Mapping a virtual address\n");
    void* virt_addr = (void*)0xFFFF800000300000;
    mapPage(virt_addr, (void*)phys_page, 0x03);

    serial_io_printf("Testing Reading and writing\n");
    printf("Testing Reading and writing");
    *(volatile uint64_t*)virt_addr = 0xDEADBEEFCAFEBABE;
    uint64_t value = *(volatile uint64_t*)virt_addr;

    if (value == 0xDEADBEEFCAFEBABE) {
        printf("Mapping and read/write test PASSED!\n");
        printf("0x%lx\n", value);  // prints lowercase hex
        printf("\n");
    } else {
        printf("Mapping failed!\n");
    }

    
}

void task_a() {
    for (int count = 0; count < 13; count++) {
        printf("[TASK A] Hello from Task A! Task ID %d\n", count);
        yield();
    }
    // After finishing, kill self to free task slot
    task_kill(multitasking_get_current_task());

    // Halt here — should never return, but just in case
    while (1) {
        yield();
    }
}

void task_b() {
    for (int count = 0; count < 13; count++) {
        printf("[TASK B] Hello from Task B! Task ID %d\n", count);
        yield();
    }
    // After finishing, kill self
    task_kill(multitasking_get_current_task());

    while (1) {
        yield();
    }
}

extern uint8_t kernel_start[];
extern uint8_t kernel_end[];

void map_kernel() {
    uintptr_t start = (uintptr_t)kernel_start;
    uintptr_t end = (uintptr_t)kernel_end;
    uintptr_t hhdm_offset = hhdm_request.response->offset;
    for (uintptr_t addr = start; addr < end; addr += 0x1000) {
        mapPage((void*)addr, (void*)(addr - hhdm_offset), 0x03);
    }
}

// this is the KFC Kernel's entry point.
void kmain(void) {
    enable_sse(); // Must be at this location
    //map_kernel();
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
      // no local variable
    initialize_terminal(framebuffer);

    //struct flanterm_context *ft_ctx = initialize_terminal(framebuffer);

    initPML4();
    check_paging();
    serial_io_init();
    serial_io_printf("Serial IO printf initialized!\n");
    serial_io_printf("SSE OK\n");
    print_cpu_info();

    // Check Limine memmap and HHDM response validity first
    if (memmap_request.response == NULL || hhdm_request.response == NULL) {
        printf("Limine memmap or HHDM response is NULL. Halting.\n");
        for (;;) __asm__("hlt");
    }
    
    
    printf("Initializing PMM and heap\n");
    printf("Initing PMM\n");
    setMemoryMap(4);
    allocator_init();

    __asm__ volatile ("cli"); // Just verify, so GDT dont go doggass
    initiateGDT();            // Set up segmentation
    remap_pic(0x20, 0x28);    // Map IRQ0–15 to INT 32–47
    init_idt();               // Set IDT gates (like set_idt_gate(32, isr32))
    timer_init(700000000ULL);
    enable_interrupts();      // STI
    multitasking_init();

    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }

    kprint("DoorsOS Kernel Booted!\n");
    kprint("Welcome to DoorsOS!\n");
    printf("Framebuffer address: %p\n", framebuffer->address);
 
    printf("Initiaiting memory management\n");
    printf("Now testing pmm malloc(k_malloc)\n");

    void* sugma = k_malloc(10);
    printf("Address: 0x%lX\n", (uintptr_t)sugma);
    k_free(sugma);

    struct meminfo mem_info = get_memory_info();
    printf("\nTesting memory map\n");

    test_page_mapping();
    printf("\n");

    printf("Stressing the allocator_malloc()\n");
    allocator_init(); // Bug FIX, after test_page_mapping, this idk why turns off

    lspci();
    fat32_mount(2048, false);
    clear_screen();
    printf("Now testing multitasking\n\n\n");
    task_create(task_a);
    task_create(task_b);

    yield(); // start first task (no return)
    printf("Clearing Screen In 3 seconds\n");
    timer_sleep_ms(2000);
    printf("1 ");
    timer_sleep_ms(2000);
    printf("2 ");
    timer_sleep_ms(1000);
    printf("3 \n");
    timer_sleep_ms(780);
    clear_screen();
    //printf("Initiating RTL8139\n");
    //rtl8139_init();
    ps2_kbio_init();
    
    print_doors_logo();
    printf("\n");
    kprint_color("Welcome to DoorsOS", COLOR_RGB_GREEN, true, COLOR_BLACK, false);
    printf("\nThis has true color enabled with RGB support, run sghsc to see it's proof.\nYou can find the source code at \"https://github.com/afifafifafifafifali/DoorsOS\"\n");
    minimal_bash();
    hcf();
}