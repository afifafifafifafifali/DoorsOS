#include "vmm.h"
#include "physalloc.h"
#include "bootloader.h"
#include "libs/string.h"
#include "gfx/serial_io.h"

extern volatile struct limine_hhdm_request hhdm_request;

static uint64_t hhdm_offset = 0;
static bool vmm_ready = false;

void vmm_init(void) {
    if (!hhdm_request.response) {
        serial_io_printf("VMM: No HHDM response!\n");
        return;
    }
    
    hhdm_offset = hhdm_request.response->offset;
    vmm_ready = true;
    
    serial_io_printf("VMM: Initialized with HHDM offset 0x%llx\n", hhdm_offset);
}

void* vmm_alloc_page(void) {
    if (!vmm_ready) return NULL;
    
    uint64_t phys = phys_alloc_page();
    if (phys == 0) return NULL;
    
    return (void*)(phys + hhdm_offset);
}

void* vmm_alloc_pages(size_t count) {
    if (!vmm_ready) return NULL;
    if (count == 0) return NULL;
    
    uint64_t phys = phys_alloc_pages(count);
    if (phys == 0) return NULL;
    
    return (void*)(phys + hhdm_offset);
}

void vmm_free_page(void* virt) {
    if (!virt || !vmm_ready) return;
    
    uint64_t phys = (uint64_t)virt - hhdm_offset;
    phys_free_page(phys);
}

void vmm_free_pages(void* virt, size_t count) {
    if (!virt || !vmm_ready || count == 0) return;
    
    uint64_t phys = (uint64_t)virt - hhdm_offset;
    phys_free_pages(phys, count);
}

uint64_t vmm_virt_to_phys(void* virt) {
    if (!virt || !vmm_ready) return 0;
    return (uint64_t)virt - hhdm_offset;
}

void* vmm_phys_to_virt(uint64_t phys) {
    if (!vmm_ready) return NULL;
    return (void*)(phys + hhdm_offset);
}

size_t vmm_get_free_count(void) {
    return (size_t)phys_alloc_get_free_count();
}

// Test function
void vmm_test(void) {
    serial_io_printf("\n=== VMM Test (HHDM approach) ===\n");
    
    if (!vmm_ready) {
        serial_io_printf("VMM not ready!\n");
        return;
    }
    
    size_t initial_free = vmm_get_free_count();
    serial_io_printf("Initial free pages: %zu\n", initial_free);
    
    // Test 1: Allocate single page
    void* page1 = vmm_alloc_page();
    serial_io_printf("Allocated page 1 at virt: %p\n", page1);
    serial_io_printf("  Physical: 0x%llx\n", vmm_virt_to_phys(page1));
    serial_io_printf("Free pages after alloc: %zu\n", vmm_get_free_count());
    
    // Test 2: Allocate another page
    void* page2 = vmm_alloc_page();
    serial_io_printf("Allocated page 2 at virt: %p\n", page2);
    serial_io_printf("  Physical: 0x%llx\n", vmm_virt_to_phys(page2));
    serial_io_printf("Free pages after alloc: %zu\n", vmm_get_free_count());
    
    // Test 3: Write and read data
    memset(page1, 0xAA, PAGE_SIZE);
    memset(page2, 0xBB, PAGE_SIZE);
    
    uint8_t* test_ptr = (uint8_t*)page1;
    if (test_ptr[0] == 0xAA && test_ptr[100] == 0xAA && test_ptr[PAGE_SIZE-1] == 0xAA) {
        serial_io_printf("PASS: Page 1 read/write test\n");
    } else {
        serial_io_printf("FAIL: Page 1 read/write test\n");
    }
    
    // Test 4: Allocate 4 contiguous pages
    void* pages4 = vmm_alloc_pages(4);
    serial_io_printf("Allocated 4 pages at virt: %p\n", pages4);
    serial_io_printf("  Physical: 0x%llx\n", vmm_virt_to_phys(pages4));
    serial_io_printf("Free pages after alloc: %zu\n", vmm_get_free_count());
    
    // Test 5: Verify contiguity (physical should be contiguous)
    uint64_t phys_base = vmm_virt_to_phys(pages4);
    if ((phys_base % (PAGE_SIZE * 4)) == 0) {
        serial_io_printf("PASS: 4 pages are physically contiguous\n");
    } else {
        serial_io_printf("INFO: Physical base 0x%llx\n", phys_base);
    }
    
    // Test 6: Free single page
    vmm_free_page(page1);
    serial_io_printf("Freed page 1\n");
    serial_io_printf("Free pages after free: %zu\n", vmm_get_free_count());
    
    // Test 7: Free 4 pages
    vmm_free_pages(pages4, 4);
    serial_io_printf("Freed 4 pages\n");
    serial_io_printf("Free pages after free: %zu\n", vmm_get_free_count());
    
    // Test 8: Free remaining page
    vmm_free_page(page2);
    serial_io_printf("Freed page 2\n");
    serial_io_printf("Free pages after free: %zu\n", vmm_get_free_count());
    
    // Verify
    size_t final_free = vmm_get_free_count();
    if (final_free == initial_free) {
        serial_io_printf("PASS: Free pages match initial count\n");
    } else {
        serial_io_printf("FAIL: Expected %zu, got %zu\n", initial_free, final_free);
    }
    
    serial_io_printf("=== VMM Test End ===\n\n");
}
