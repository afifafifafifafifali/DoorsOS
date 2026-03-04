#ifndef PHYS_ALLOC_H
#define PHYS_ALLOC_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

void phys_alloc_init(void);

// Reserve a physical memory region (mark as used)
void phys_alloc_reserve(uint64_t phys_base, size_t size);

// Allocate single physical page (returns physical address)
uint64_t phys_alloc_page(void);

// Free single physical page
void phys_free_page(uint64_t phys_addr);

// Allocate multiple contiguous physical pages
uint64_t phys_alloc_pages(size_t count);

// Free multiple contiguous physical pages
void phys_free_pages(uint64_t phys_base, size_t count);

// Get count of free physical pages
uint64_t phys_alloc_get_free_count(void);

// Test function
void phys_alloc_test(void);

#endif
