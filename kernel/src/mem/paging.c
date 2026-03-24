#include "paging.h"
#include "../gfx/serial_io.h"
#include "new/pmm.h"
#include "heap.h"
#include "../bootloader.h"

volatile PageTable* pml4;

void flushTLB(void* page) {
    __asm__ volatile ("invlpg (%0)" :: "r" (page) : "memory");
}

uint64_t readCR3(void) {
    uint64_t val;
    __asm__ volatile ( "mov %%cr3, %0" : "=r"(val) );
    return val;
}

PageTable* initPML4() {
    uintptr_t cr3 = readCR3(); // CRISTINO ROILDO
    uintptr_t phys_pml4 = cr3 & ~0xFFFUL;
    uintptr_t hhdm = hhdm_request.response->offset;
    pml4 = (PageTable*)(hhdm + phys_pml4);
    serial_io_printf("PML4 virtual address: %p\n", pml4);
    return pml4;
}

void setPageTableEntry(PageEntry* entry, uint8_t flags, uintptr_t physical_address, uint16_t available) {
    entry->present = (flags >> 0) & 1;
    entry->writable = (flags >> 1) & 1;
    entry->user_accessible = (flags >> 2) & 1;
    entry->write_through_caching = (flags >> 3) & 1;
    entry->disable_cache = (flags >> 4) & 1;
    entry->null = (flags >> 5) & 1;
    entry->global = (flags >> 6) & 1;
    entry->no_execute = (flags >> 7) & 1;
    entry->physical_address = physical_address;
    entry->avl1 = available & 0x3;
    entry->avl2 = available >> 3;
}

static void allocateEntry(PageTable* table, size_t index, uint8_t flags) {
     serial_io_printf("[SHITTY CATHOLIC PAGING MANAGEMENT  SYSTEM] allocateEntry: index=%lu\n", index);
    void* virt_addr = allocator_malloc(4096);
    if (!virt_addr) {
         serial_io_printf("[SHITTY CATHOLIC PAGING MANAGEMENT  SYSTEM ] allocateEntry: k_malloc failed\n");
        while(1) __asm__("hlt");
    }
     serial_io_printf("[SHITTY CATHOLIC PAGING MANAGEMENT  SYSTEM ] allocateEntry: virt_addr=%p\n", virt_addr);
    
    uintptr_t hhdm = hhdm_request.response->offset;
    uintptr_t phys_addr = (uintptr_t)virt_addr - hhdm;
     serial_io_printf("[SHITTY CATHOLIC PAGING MANAGEMENT  SYSTEM ] allocateEntry: phys_addr=0x%lx\n", phys_addr);
    
    setPageTableEntry(&(table->entries[index]), flags, phys_addr >> 12, 0);
    
    for (int i = 0; i < 4096; i++) {
        ((uint8_t*)virt_addr)[i] = 0;
    }
    serial_io_printf("[SHITTY CATHOLIC PAGING MANAGEMENT  SYSTEM ] allocateEntry: done\n");
}

void mapPage(void* virtual_address, void* physical_address, uint8_t flags) {
    serial_io_printf("[ SHITTY CATHOLIC PAGING MANAGEMENT  SYSTEM ] mapPage called: virt=%p phys=%p\n", virtual_address, physical_address);
    
    uintptr_t virtual_address_int = (uintptr_t)virtual_address;
    uintptr_t physical_address_int = (uintptr_t)physical_address;
    uintptr_t hhdm = hhdm_request.response->offset;

    uint64_t pml4_index = (virtual_address_int >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_address_int >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_address_int >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_address_int >> 12) & 0x1FF;

    if (!pml4->entries[pml4_index].present)
        allocateEntry(pml4, pml4_index, flags);

    PageTable* pdpt = (PageTable*)(hhdm + (pml4->entries[pml4_index].physical_address << 12));

    if (!pdpt->entries[pdpt_index].present) 
        allocateEntry(pdpt, pdpt_index, flags);

    PageTable* pd = (PageTable*)(hhdm + (pdpt->entries[pdpt_index].physical_address << 12));

    if (!pd->entries[pd_index].present) 
        allocateEntry(pd, pd_index, flags);

    PageTable* pt = (PageTable*)(hhdm + (pd->entries[pd_index].physical_address << 12));

    setPageTableEntry(&(pt->entries[pt_index]), flags, physical_address_int >> 12, 0);

    flushTLB(virtual_address);
}

void* getPhysicalAddress(void* virtual_address) {
    uintptr_t address = (uintptr_t)virtual_address;
    uintptr_t hhdm = hhdm_request.response->offset;

    uint64_t offset = address & 0xFFF;
    uint64_t pt_index = (address >> 12) & 0x1FF;
    uint64_t pd_index = (address >> 21) & 0x1FF;
    uint64_t pdpt_index = (address >> 30) & 0x1FF;
    uint64_t pml4_index = (address >> 39) & 0x1FF;

    PageTable* pdpt = (PageTable*)(hhdm + (pml4->entries[pml4_index].physical_address << 12));
    PageTable* pd = (PageTable*)(hhdm + (pdpt->entries[pdpt_index].physical_address << 12));
    PageTable* pt = (PageTable*)(hhdm + (pd->entries[pd_index].physical_address << 12));

    return (void*)((pt->entries[pt_index].physical_address << 12) + offset);
}


void unmapPage(void* virtual_address) {
    serial_io_printf("[ SHITTY CATHOLIC PAGING MANAGEMENT SYSTEM ] unmapPage called: virt=%p\n", virtual_address);

    uintptr_t virtual_address_int = (uintptr_t)virtual_address;
    uintptr_t hhdm = hhdm_request.response->offset;

    uint64_t pml4_index = (virtual_address_int >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_address_int >> 30) & 0x1FF;
    uint64_t pd_index   = (virtual_address_int >> 21) & 0x1FF;
    uint64_t pt_index   = (virtual_address_int >> 12) & 0x1FF;

    if (!pml4->entries[pml4_index].present) {
        serial_io_printf("[ SHITTY CATHOLIC PAGING MANAGEMENT SYSTEM ] unmapPage: PML4 entry not present\n");
        return;
    }
    PageTable* pdpt = (PageTable*)(hhdm + (pml4->entries[pml4_index].physical_address << 12));

    if (!pdpt->entries[pdpt_index].present) {
        serial_io_printf("[ SHITTY CATHOLIC PAGING MANAGEMENT SYSTEM ] unmapPage: PDPT entry not present\n");
        return;
    }
    PageTable* pd = (PageTable*)(hhdm + (pdpt->entries[pdpt_index].physical_address << 12));

    if (!pd->entries[pd_index].present) {
        serial_io_printf("[ SHITTY CATHOLIC PAGING MANAGEMENT SYSTEM ] unmapPage: PD entry not present\n");
        return;
    }
    PageTable* pt = (PageTable*)(hhdm + (pd->entries[pd_index].physical_address << 12));

    // Clear the PT entry
    memset(&pt->entries[pt_index], 0, sizeof(PageEntry));
    flushTLB(virtual_address);

    serial_io_printf("[ SHITTY CATHOLIC PAGING MANAGEMENT SYSTEM ] unmapPage: done\n");
}

void unmapPages(void* virtual_address, uint64_t size) {
    uint64_t pages = (size + 4095) / 4096;
    for (uint64_t i = 0; i < pages; i++)
        unmapPage((void*)((uintptr_t)virtual_address + i * 4096));
}