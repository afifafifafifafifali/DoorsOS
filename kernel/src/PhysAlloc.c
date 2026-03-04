#include "physalloc.h"
#include "bootloader.h"
#include "mem/new/pmm.h"
#include "libs/string.h"
#include "gfx/serial_io.h"
#include "info/cpuinfo.h" // for uint64_t memory_amount


extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;

#define BITMAP_MAX_PAGES 65536  // Track up to 256 MiB of RAM
#define BITMAP_SIZE ((BITMAP_MAX_PAGES + 7) / 8)

static uint8_t bitmap[BITMAP_SIZE];
static uint64_t total_pages = 0;
static uint64_t free_pages = 0;

// HHDM offset for virtual-physical conversion
static uint64_t hhdm_offset = 0;

static void bitmap_set(size_t bit) {
    bitmap[bit / 8] |= (1 << (bit % 8));
}

static void bitmap_clear(size_t bit) {
    bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static int bitmap_test(size_t bit) {
    return (bitmap[bit / 8] >> (bit % 8)) & 1;
}

void phys_alloc_init(void) {
    hhdm_offset = hhdm_request.response->offset;
    
    memset(bitmap, 0, sizeof(bitmap));
    total_pages = 0;
    free_pages = 0;
    
    if (!memmap_request.response) {
        serial_io_printf("PhysAlloc: No memmap!\n");
        return;
    }
    
    // Reserve page 0 (NULL page - catch NULL dereferences)
    bitmap_set(0);
    
    // Count usable pages
    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap_request.response->entries[i];
        
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            uint64_t base = entry->base;
            uint64_t len = entry->length;
            
            // Align to page boundary
            uint64_t aligned_base = (base + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
            uint64_t aligned_len = len - (aligned_base - base);
            
            if (aligned_len >= PAGE_SIZE) {
                uint64_t pages = aligned_len / PAGE_SIZE;
                total_pages += pages;
                free_pages += pages;
            }
        }
    }
    
    serial_io_printf("PhysAlloc: Initialized with %llu free pages (%llu MiB)\n", 
                     free_pages, (free_pages * PAGE_SIZE) / (1024 * 1024));
}

void phys_alloc_reserve(uint64_t phys_base, size_t size) {
    // Mark physical pages as used in bitmap
    uint64_t start_page = phys_base / PAGE_SIZE;
    uint64_t page_count = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    
    for (uint64_t i = 0; i < page_count && (start_page + i) < BITMAP_MAX_PAGES; i++) {
        if (!bitmap_test(start_page + i)) {
            bitmap_set(start_page + i);
            if (free_pages > 0) free_pages--;
        }
    }
    
    serial_io_printf("PhysAlloc: Reserved %llx bytes at %llx\n", size, phys_base);
}

uint64_t phys_alloc_page(void) {
    for (uint64_t i = 0; i < BITMAP_MAX_PAGES; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            if (free_pages > 0) free_pages--;
            return i * PAGE_SIZE;
        }
    }
    
    serial_io_printf("PhysAlloc: Out of memory!\n");
    return 0;
}

void phys_free_page(uint64_t phys_addr) {
    if (phys_addr == 0) return;
    
    uint64_t page = phys_addr / PAGE_SIZE;
    if (page < BITMAP_MAX_PAGES && bitmap_test(page)) {
        bitmap_clear(page);
        free_pages++;
    }
}

uint64_t phys_alloc_pages(size_t count) {
    if (count == 0) return 0;
    if (count > free_pages) return 0;
    
    // Find contiguous free pages
    for (uint64_t i = 0; i < BITMAP_MAX_PAGES - count; i++) {
        int found = 1;
        for (size_t j = 0; j < count; j++) {
            if (bitmap_test(i + j)) {
                found = 0;
                break;
            }
        }
        
        if (found) {
            for (size_t j = 0; j < count; j++) {
                bitmap_set(i + j);
            }
            free_pages -= count;
            return i * PAGE_SIZE;
        }
    }
    
    serial_io_printf("PhysAlloc: Cannot allocate %zu contiguous pages\n", count);
    return 0;
}

void phys_free_pages(uint64_t phys_base, size_t count) {
    if (phys_base == 0 || count == 0) return;
    
    uint64_t start_page = phys_base / PAGE_SIZE;
    for (size_t i = 0; i < count && (start_page + i) < BITMAP_MAX_PAGES; i++) {
        if (bitmap_test(start_page + i)) {
            bitmap_clear(start_page + i);
            free_pages++;
        }
    }
}

uint64_t phys_alloc_get_free_count(void) {
    return free_pages;
}

// Test function
void phys_alloc_test(void) {
    serial_io_printf("\n=== PhysAlloc Test ===\n");
    
    uint64_t initial_free = phys_alloc_get_free_count();
    serial_io_printf("Initial free pages: %llu\n", initial_free);
    
    // Test 1: Allocate single page
    uint64_t page1 = phys_alloc_page();
    serial_io_printf("Allocated page 1 at: 0x%llx\n", page1);
    serial_io_printf("Free pages after alloc: %llu\n", phys_alloc_get_free_count());
    
    // Test 2: Allocate another page
    uint64_t page2 = phys_alloc_page();
    serial_io_printf("Allocated page 2 at: 0x%llx\n", page2);
    serial_io_printf("Free pages after alloc: %llu\n", phys_alloc_get_free_count());
    
    // Test 3: Free first page
    phys_free_page(page1);
    serial_io_printf("Freed page 1\n");
    serial_io_printf("Free pages after free: %llu\n", phys_alloc_get_free_count());
    
    // Test 4: Allocate 4 contiguous pages
    uint64_t pages4 = phys_alloc_pages(4);
    serial_io_printf("Allocated 4 pages at: 0x%llx\n", pages4);
    serial_io_printf("Free pages after alloc: %llu\n", phys_alloc_get_free_count());
    
    // Test 5: Free the 4 pages
    phys_free_pages(pages4, 4);
    serial_io_printf("Freed 4 pages\n");
    serial_io_printf("Free pages after free: %llu\n", phys_alloc_get_free_count());
    
    // Test 6: Free remaining page
    phys_free_page(page2);
    serial_io_printf("Freed page 2\n");
    serial_io_printf("Free pages after free: %llu\n", phys_alloc_get_free_count());
    
    // Verify
    uint64_t final_free = phys_alloc_get_free_count();
    if (final_free == initial_free) {
        serial_io_printf("PASS: Free pages match initial count\n");
    } else {
        serial_io_printf("FAIL: Expected %llu, got %llu\n", initial_free, final_free);
    }
    
    serial_io_printf("=== PhysAlloc Test End ===\n\n");


    serial_io_printf("PMM basic test...\n");

    uint64_t p1 = phys_alloc_page();
    uint64_t p2 = phys_alloc_page();

    if (!p1 || !p2) {
        serial_io_printf("FAIL: allocation returned NULL\n");
        return;
    }

    if (p1 == p2) {
        serial_io_printf("FAIL: duplicate page allocated\n");
        return;
    }

    if (p1 & 0xFFF || p2 & 0xFFF) {
        serial_io_printf("FAIL: page not 4K aligned\n");
        return;
    }

    phys_free_page(p1);
    phys_free_page(p2);

    serial_io_printf("========PASS: basic test\n");
}
