#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "gfx/term.h"
#include "flanterm/src/flanterm.h"
#include "flanterm/src/flanterm_backends/fb.h"
#include "interrupts/isr.h"
#include "fs/ahci.h"
#include "info/cpuinfo.h"
#include "interrupts/pit.h"
#include "fs/detect_ahci.h"
#include "mem/new/pmm.h"
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
#include "bootloader.h"
#include "exe/dsc.h"
#include "mem/paging.h"
#include "mem/heap.h"
#include <limine.h>
//#define HEAP_SIZE (400UL * 1024UL * 1024UL) // 400 MIB
//#define HEAP_SIZE (100UL * 1024UL * 1024UL)
//uint8_t heap[HEAP_SIZE];  // Define the heap array
// Set the base revision to 3, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
volatile LIMINE_BASE_REVISION(3);

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

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

void test_sleep() {
    clear_screen();
    printf("Initializing DoorsOS...\n");
    print_doors_logo();
    printf("Welcome to DoorsOS!\n");
}

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





void minimal_bash() {
    while (1) {
        kprint("root@testpc# ");

        string_t input = (string_t)malloc(256);
        string_t result = ps2_kbio_read(input, 256);

        if (result != NULL) {
            if (strEql(result, "ata_identify")) {
                printf("\n");
                identify();
            }
            else if (strEql(result, "neofetch")) {
                printf("\n");
                print_doors_logo();
            }
            else if (strEql(result, "clear")) {
                printf("\n");
                clear_screen();
            }
            else if (strEql(result, "paging")) {
                printf("\n");
                check_paging();
            }
            else if (strEql(result, "sse_test")) {
                printf("\n");
                test_sse();
            }
            else if (strEql(result, "exit")) {
                printf("\nExiting...\n");
                free(input);
                free(result);
                return;
            }
            else if (strncmp(result, "echo ", 5) == 0) {
                printf("\n%s\n", result + 5);
            }
            else if(strncmp(result, "ls " , 3) == 0) {
                string_t pathy = result+3;
                if(strEql(pathy,"/")) {
                    printf("\n");
                    fat32_list_root();
                }
                else if(strEql(pathy, " ")) {
                    printf("\n");
                    if(strEql(current_directory,"/")){
                        fat32_list_root();
                    }
                    else{
                        ls(NULL);
                    }
                }
                else {
                    printf("\n");
                    ls(pathy);
                }
            }
            else if(strncmp(result, "cat ", 4) == 0) {
                string_t sugma = result + 4;
                if (strEql(sugma, " ")){
                    printf("\n Missing 1 required argument, filename\n");
                }else {
                    printf("\n");
                    cat(sugma);
                }
            }
            else if(strncmp(result, "mkfile ", 7) == 0) {
                string_t filename = result + 7;  // skip "mkfile "

                printf("Enter contents for %s:\n> ", filename);

    // Allocate buffer for input content
                string_t content = (string_t)malloc(1024);  // or any reasonable max size
                memset(content, 0, 1024);

                 // Read keyboard input for file contents
                string_t entered = ps2_kbio_read(content, 1024);

                printf("\nCreating file '%s'...\n", filename);

    // Write to file
                if (fat32_write_file(filename, (const uint8_t*)entered, strlen(entered))) {
                     printf("File '%s' created successfully.\n", filename);
                } else {
                    printf("Failed to create file '%s'.\n", filename);
                }

    // Free allocated memory
                free(content);
            }

            else {
                printf("\nInvalid Command %s\n", result);
            }
        } else {
            kprint("Failed to read input!\n");
        }
    }
}


static inline uint64_t read_cr0(void) {
    uint64_t val;
    __asm__ volatile("mov %%cr0, %0" : "=r"(val));
    return val;
}


void check_paging() {
    uint64_t cr0 = read_cr0();
    if (cr0 & (1ULL << 31)) {
        kprint("Paging is enabled.\n");
    } else {
        kprint("Paging is NOT enabled.\n");
    }
}



void switch_to_user_mode(void* user_entry, void* user_stack) {
    // We'll push SS, RSP, RFLAGS, CS, RIP in that order, then iretq.
    asm volatile (
        "cli\n"                      // Disable interrupts

        "mov $0x23, %%ax\n"          // User data segment selector (RPL 3)
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"

        "pushq $0x23\n"              // User SS
        "pushq %0\n"                 // User RSP
        "pushfq\n"                   // Push RFLAGS
        "popq %%rax\n"
        "or $0x200, %%rax\n"         // Set IF flag (bit 9)
        "pushq %%rax\n"
        "pushq $0x1B\n"              // User CS
        "pushq %1\n"                 // User RIP (entry point)

        "iretq\n"                    // Return to user mode
        :
        : "r"(user_stack), "r"(user_entry)
        : "rax"
    );
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

void disable_paging() {
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 &= ~(1ULL << 31); // Clear PG bit (bit 31)
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
}

void my_mouse_callback(MouseEvent* ev) {
    if (ev->type == MOUSE_EVENT_MOVE) {
        // Draw cursor at ev->x, ev->y
        printf("\033[%d;%dHX", ev->y + 1, ev->x + 1);
    }
}


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
    void* virt_addr = (void*)0xFFFF800000300000;
    mapPage(virt_addr, (void*)phys_page, 0x03);

    serial_io_printf("Testing Reading and writing\n");
    *(volatile uint64_t*)virt_addr = 0xDEADBEEFCAFEBABE;
    uint64_t value = *(volatile uint64_t*)virt_addr;

    if (value == 0xDEADBEEFCAFEBABE) {
        serial_io_printf("Mapping and read/write test PASSED!\n");
    } else {
        serial_io_printf("Mapping failed!\n");
    }
}


// The following will be our kernel's entry point.
void kmain(void) {
    enable_sse();
    //initiatePMM(PHYS_MEM_START, PHYS_MEM_SIZE);
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];
    struct flanterm_context *ft_ctx = initialize_terminal(framebuffer);
    //allocator_init(heap, HEAP_SIZE);
    
    kprint("Heap initialized!\n");
    
    initPML4();
    //disable_paging();
   check_paging();
    serial_io_init();
    serial_io_printf("Serial IO printf initialized!\n");
   

   serial_io_printf("SSE OK\n");
   print_cpu_info();
  
   printf("Initializing PMM and heap\n");

// Check Limine memmap and HHDM response validity first
if (memmap_request.response == NULL || hhdm_request.response == NULL) {
    printf("Limine memmap or HHDM response is NULL. Halting.\n");
    for (;;) __asm__("hlt");
}

printf("Initing PMM\n");


// Initialize your physical memory manager if you have one
setMemoryMap(4);
allocator_init();
    // Uncomment the following line to test SSE functionality.
    // This will double the value 21.0f and print the result.
    // If the output is 42, then SSE is working correctly.
   // test_sse();
   __asm__ volatile ("cli");

    
    initiateGDT();

    remap_pic(0x20, 0x28);
    enable_interrupts();
    init_idt();

    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
        hcf();
    }

    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        hcf();
    }
    mouse_init();
    mouse_set_callback(my_mouse_callback);
    kprint("DoorsOS Kernel Booted!\n");
    printf("Initiating PMM\n");
    serial_io_printf("Initializing PMM\n");
    kprint("Welcome to DoorsOS!\n");
    printf("Framebuffer address: %p\n", framebuffer->address);
    
    
    printf("Initiaitg memory mgmt\n");
    printf("Bonnafide Now malloc\n");

    void* sugma = k_malloc(10);
    printf("Address: 0x%lX\n", (uintptr_t)sugma);

    struct meminfo mem_info = get_memory_info();
    printf("\n Now gonna test memory map");
    test_page_mapping();
lspci();

fat32_mount(2048, false);
ps2_kbio_init();
kprint("PS/2 Keyboard Driver Initialized!\n");
minimal_bash();
hcf();

}
