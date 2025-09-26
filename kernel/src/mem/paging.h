#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

// options1:     R P/W U/S PWT PCD A D PS(1) G AVL
// Bits (0-11):  1 1   1   1   1   1 1 1     1 3   
   
// bits (12-51): address (significant bits are 0 by default,)

// options2:        AVL PK XD
// Bits (52-63):    7   4  1 


#define PAGE_PRESENT        0x1ULL          // Page present in memory
#define PAGE_WRITE          0x2ULL          // Read/write if set, read-only if clear
#define PAGE_USER           0x4ULL          // User-mode accessible if set
#define PAGE_WRITE_THROUGH  0x8ULL          // Write-through caching
#define PAGE_CACHE_DISABLE  0x10ULL         // Disable caching for this page
#define PAGE_ACCESSED       0x20ULL         // Set by CPU on access
#define PAGE_DIRTY          0x40ULL         // Set by CPU on write
#define PAGE_HUGE           0x80ULL         // Huge page (2MB/1GB) if set
#define PAGE_GLOBAL         0x100ULL        // Global TLB entry if set
#define PAGE_NO_EXECUTE     (1ULL << 63)    // No-execute (if supported)

typedef struct PageEntry {
    uint8_t present : 1;
    uint8_t writable : 1;
    uint8_t user_accessible : 1;
    uint8_t write_through_caching : 1;
    uint8_t disable_cache : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t null : 1;
    uint8_t global : 1;
    uint8_t avl1 : 3;
    uintptr_t physical_address : 40;
    uint16_t avl2 : 11;
    uint8_t no_execute : 1;
} PageEntry;

typedef struct PageTable {
    PageEntry entries[512];
} PageTable;

void flushTLB(void* page);
void* getPhysicalAddress(void* virtual_address); 
PageTable* initPML4(void); 
void mapPage(void* virtual_address, void* physical_address, uint8_t flags);
uint64_t readCR3(void);
// add to mem/new/pmm.h (or mem/pmm.h if that's included by other files)

#endif