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
static const char* getMemoryMapType(uint32_t type) {
    switch (type) {
        case 0x1:     return "Usable RAM";
        case 0x2:     return "Reserved";
        case 0x3:     return "ACPI reclaimable";
        case 0x4:     return "ACPI NVS";
        case 0x5:     return "Bad memory";
        case 0x1000:  return "Bootloader reclaimable";
        case 0x1001:  return "Kernel/Modules";
        case 0x1002:  return "Framebuffer";
        default:      return "???";
    }
}

// ------------------------------
// Print Usable Memory Maps
// ------------------------------
void printMemoryMaps() {
    memmap_info = memmap_request.response;

    for (size_t i = 0; i < memmap_info->entry_count; i++) {
        struct limine_memmap_entry* entry = memmap_info->entries[i];

        switch (entry->type) {
            case 0x1:
            case 0x1000:
            case 0x3: {
                serial_io_printf("Entry: ");
                serial_io_printf(i);          // Custom number print
                serial_io_printf("  Type: ");
                serial_io_printf(getMemoryMapType(entry->type));

                serial_io_printf("\n  Base: ");
                serial_io_printf(entry->base);

                serial_io_printf("  Length: ");
                serial_io_printf(entry->length);
                serial_io_printf("\n");
                break;
            }
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

// ------------------------------
// Buddy-like Allocator (Recursive)
// ------------------------------
static void* b_malloc(uint64_t* base, size_t length, size_t size) {
    if (length <= BLOCK_SIZE) {
        if (size + 1 <= length && *((uint64_t*) base) == 0) {
            *base = size;
            memset(base + 1, 0, sizeof(void*) * size);
            return (void*) (base + 1);
        }
        return NULL;
    }

    size_t half = length / 2;

    if (half <= size + 1 && *((uint64_t*) base) == 0) {
        *base = size;
        memset(base + 1, 0, sizeof(void*) * size);
        return (void*) (base + 1);
    } else if (half > size) {
        void* b = b_malloc(base, half, size);
        if (b == NULL) {
            b = b_malloc(base + half, half, size);
        }
        return b;
    }

    return NULL;
}

static inline void* phys_to_virt(uint64_t phys_addr) {
    return (void*)(hhdm_request.response->offset + phys_addr);
}
// ------------------------------
// Kernel Memory Allocation API
// ------------------------------
void* k_malloc(size_t size) {
    void* virt_base = phys_to_virt(memmap->base);
    return b_malloc((uint64_t*) virt_base, memmap->length, size);
}

void k_free(void* base) {
    uint64_t size = *(((uint64_t*) base) - 1);
    memset(base, 0, size);
}
