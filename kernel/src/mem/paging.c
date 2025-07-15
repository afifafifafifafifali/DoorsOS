#include "paging.h"
#include "new/pmm.h"
#include "../bootloader.h" // For hhdm_request

static PageTable* pml4; // Virtual pointer to PML4 table

// Flush the TLB entry for a given virtual page
void flushTLB(void* page) 
{
    __asm__ volatile ("invlpg (%0)" :: "r" (page) : "memory");
}

// Read CR3 register (physical address of PML4)
uint64_t readCR3(void)
{
    uint64_t val;
    __asm__ volatile ( "mov %%cr3, %0" : "=r"(val) );
    return val;
}

// Initialize the pml4 pointer (virtual address via HHDM)
PageTable* initPML4() {
    uintptr_t cr3 = readCR3();

    // Align physical address to page boundary
    uintptr_t phys_pml4 = cr3 & ~0xFFFUL;

    // Get HHDM offset from Limine bootloader
    uintptr_t hhdm = hhdm_request.response->offset;

    // Convert physical to virtual by adding HHDM offset
    pml4 = (PageTable*)(hhdm + phys_pml4);

    serial_io_printf("PML4 virtual address: %p\n", pml4);

    return pml4;
    extern uint8_t kernel_start;
    extern uint8_t kernel_end;
    uintptr_t phys_kernel_start = (uintptr_t)&kernel_start - hhdm_request.response->offset;
    uintptr_t phys_kernel_end   = (uintptr_t)&kernel_end - hhdm_request.response->offset;

    for (uintptr_t addr = phys_kernel_start; addr < phys_kernel_end; addr += 0x1000) {
        uintptr_t virt = addr + 0x80000000;  // higher-half kernel mapping
        mapPage((void*)virt, (void*)addr, 0b11); // present + writable
    }

}

// Set up a page table entry with flags, physical address, and available bits
void setPageTableEntry(PageEntry* entry, uint8_t flags, uintptr_t physical_address, uint16_t available) 
{
    entry->present = (flags >> 0) & 1;
    entry->writable = (flags >> 1) & 1;
    entry->user_accessible = (flags >> 2) & 1;
    entry->write_through_caching = (flags >> 3) & 1;
    entry->disable_cache = (flags >> 4) & 1;
    entry->null = (flags >> 5) & 1;
    entry->global = (flags >> 6) & 1;
    entry->no_execute = (flags >> 7) & 1;

    // Physical address stored as bits 12..51, so shift right 12
    entry->physical_address = physical_address;

    // Available bits for OS use
    entry->avl1 = available & 0x3;
    entry->avl2 = available >> 3;
}

// Translate a virtual address to the physical address it maps to
void* getPhysicalAddress(void* virtual_address) 
{
    uintptr_t address = (uintptr_t) virtual_address;
    uintptr_t hhdm = hhdm_request.response->offset;

    uint64_t offset = address & 0xFFF;
    uint64_t page_table_index = (address >> 12) & 0x1FF;
    uint64_t page_directory_index = (address >> 21) & 0x1FF;
    uint64_t page_directory_pointer_index = (address >> 30) & 0x1FF;
    uint64_t pml4_index = (address >> 39) & 0x1FF;

    PageTable* page_directory_pointer = (PageTable*) (hhdm + (pml4->entries[pml4_index].physical_address << 12));
    PageTable* page_directory = (PageTable*) (hhdm + (page_directory_pointer->entries[page_directory_pointer_index].physical_address << 12));
    PageTable* page_table = (PageTable*) (hhdm + (page_directory->entries[page_directory_index].physical_address << 12));

    return (void*) ((page_table->entries[page_table_index].physical_address << 12) + offset);
}

// Allocate a new 4KiB page and assign it to a page table entry
static void allocateEntry(PageTable* table, size_t index, uint8_t flags)
{
    void* physical_address = k_malloc(4096); // Alloc physical memory page

    // physical_address must be page-aligned, so shift right 12 to store in entry
    setPageTableEntry(&(table->entries[index]), flags, (uintptr_t)physical_address >> 12, 0);

    // Zero out the new page to avoid garbage
    uintptr_t hhdm = hhdm_request.response->offset;
    memset((void*)(hhdm + (uintptr_t)physical_address), 0, 4096);
}

// Map a virtual address to a physical address with the given flags
void mapPage(void* virtual_address, void* physical_address, uint8_t flags) 
{
    uintptr_t virtual_address_int = (uintptr_t) virtual_address;
    uintptr_t physical_address_int = (uintptr_t) physical_address;
    uintptr_t hhdm = hhdm_request.response->offset;

    uint64_t pml4_index = (virtual_address_int >> 39) & 0x1FF;
    uint64_t page_directory_pointer_index = (virtual_address_int >> 30) & 0x1FF;
    uint64_t page_directory_index = (virtual_address_int >> 21) & 0x1FF;
    uint64_t page_table_index = (virtual_address_int >> 12) & 0x1FF;

    // Allocate page directory pointer table if not present
    if (!pml4->entries[pml4_index].present)
        allocateEntry(pml4, pml4_index, flags);

    PageTable* page_directory_pointer = (PageTable*) (hhdm + (pml4->entries[pml4_index].physical_address << 12));

    // Allocate page directory if not present
    if (!page_directory_pointer->entries[page_directory_pointer_index].present) 
        allocateEntry(page_directory_pointer, page_directory_pointer_index, flags);

    PageTable* page_directory = (PageTable*) (hhdm + (page_directory_pointer->entries[page_directory_pointer_index].physical_address << 12));

    // Allocate page table if not present
    if (!page_directory->entries[page_directory_index].present) 
        allocateEntry(page_directory, page_directory_index, flags);

    PageTable* page_table = (PageTable*) (hhdm + (page_directory->entries[page_directory_index].physical_address << 12));

    // Set the page table entry for the final mapping
    setPageTableEntry(&(page_table->entries[page_table_index]), flags, physical_address_int >> 12, 0);

    // Flush the TLB for this virtual page
    flushTLB(virtual_address);
}
