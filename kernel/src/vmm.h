#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>

void vmm_init(void);

// Allocate virtual memory (returns HHDM-mapped virtual address)
void* vmm_alloc_pages(size_t count);
void* vmm_alloc_page(void);

// Free virtual memory
void vmm_free_pages(void* virt, size_t count);
void vmm_free_page(void* virt);

// Get physical address from virtual (HHDM)
uint64_t vmm_virt_to_phys(void* virt);

// Get virtual address from physical (HHDM)
void* vmm_phys_to_virt(uint64_t phys);

// Get free virtual memory count (in pages)
size_t vmm_get_free_count(void);

// Test function
void vmm_test(void);

#endif
