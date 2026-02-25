#include "pmm.h"
#include "../../libs/string.h"
#include "../../gfx/printf.h"
#include "../../bootloader.h"
#include "bitmap.h"
#include <stdint.h>
#include <stddef.h>

// ------------------------------
// Externals from Limine
// ------------------------------
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request    hhdm_request;

static struct limine_memmap_response* memmap_info;
static struct limine_memmap_entry*    memmap;

// ------------------------------
// Memory Map Type Strings
// ------------------------------
static const char* getMemoryMapType(uint64_t type) {
    switch (type) {
        case LIMINE_MEMMAP_USABLE:
            return "Usable RAM";

        case LIMINE_MEMMAP_RESERVED:
            return "Reserved";

        case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
            return "ACPI reclaimable";

        case LIMINE_MEMMAP_ACPI_NVS:
            return "ACPI NVS";

        case LIMINE_MEMMAP_BAD_MEMORY:
            return "Bad memory";

        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
            return "Bootloader reclaimable";

        case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES:
            return "Kernel/Modules";

        case LIMINE_MEMMAP_FRAMEBUFFER:
            return "Framebuffer";

        default:
            return "Unknown";
    }
}

// ------------------------------
// Print Usable Memory Maps
// ------------------------------
void printMemoryMaps() {

    if (!memmap_request.response || !hhdm_request.response) {
        serial_io_printf("No memmap response!\n");
        return;
    }

    uintptr_t hhdm = hhdm_request.response->offset;
    struct limine_memmap_response* resp = memmap_request.response;

    for (uint64_t i = 0; i < resp->entry_count; i++) {

        struct limine_memmap_entry* entry_ptr = resp->entries[i];

        struct limine_memmap_entry* entry =
            (struct limine_memmap_entry*)(
                ((uintptr_t)entry_ptr < 0x100000000ULL)
                    ? ((uintptr_t)entry_ptr + hhdm)
                    : (uintptr_t)entry_ptr
            );

        if (!entry)
            continue;

        if (entry->type == LIMINE_MEMMAP_USABLE ||
            entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
            entry->type == LIMINE_MEMMAP_ACPI_RECLAIMABLE) {

            serial_io_printf("Entry: %llu\n", i);
            serial_io_printf("  Type: %s\n", getMemoryMapType(entry->type));
            serial_io_printf("  Base: 0x%llx\n", entry->base);
            serial_io_printf("  Length: %llu bytes\n", entry->length);
            serial_io_printf("\n");
        }
    }
}
// ------------------------------
// Memory Map Selection & Info
// ------------------------------
void setMemoryMap(uint8_t selection) {
    memmap_info = memmap_request.response;
    if (selection >= memmap_info->entry_count) {
        serial_io_printf("Invalid memmap selection %d\n", selection);
        memmap = NULL;
        return;
    }
    memmap = memmap_info->entries[selection];
}


void* getMemoryMapBase() {
    return (void*) memmap->base;
}

uint64_t getMemoryMapLength() {
    return memmap->length;
}
void* phys_to_virt(uint64_t phys_addr) {
    return (void*)(hhdm_request.response->offset + phys_addr);
}
uint64_t virt_to_phys(void* virt_addr) {
    // HHDM offset: virtual = physical + offset
    // physical = virtual - offset
    return (uint64_t)virt_addr - HHDM_BASE;
}

#include "../heap.h"

void* k_malloc(size_t size) {
    if (size == 0) return NULL;
    void *ptr = malloc(size);
    if (ptr) memset(ptr, 0, size);
    return ptr;
}

void k_free(void* base) {
   if(base) free(base);
}

void init_pmm(void) {
   
}
