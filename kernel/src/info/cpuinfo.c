#include "cpuinfo.h"
#include "../bootloader.h"
#include "../gfx/serial_io.h"
#include "../libs/string.h"
#include <stdint.h>


/* Limine requests */
extern volatile struct limine_mp_request smp_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;

/* Global storage (safe for early boot) */
char vendor[100] = {0};
char brand[49] = {0}; // global to avoid stack issues
int detected = 0;
uint64_t memory_amount = 0;

/* Safe CPUID wrapper: no RBX tricks, uses only 32-bit registers */
static inline void cpuid(uint32_t leaf,
                         uint32_t *a,
                         uint32_t *b,
                         uint32_t *c,
                         uint32_t *d)
{
    uint32_t eax_, ebx_, ecx_, edx_;
    asm volatile(
        "cpuid"
        : "=a"(eax_), "=b"(ebx_), "=c"(ecx_), "=d"(edx_)
        : "a"(leaf)
        : "memory"
    );
    *a = eax_;
    *b = ebx_;
    *c = ecx_;
    *d = edx_;
}

void return_cpu(void) {
    /* -------- CPU COUNT -------- */
    if (!smp_request.response) {
        detected = 1; // fallback: BSP only
    } else {
        detected = smp_request.response->cpu_count;
    }

    /* -------- CPU BRAND STRING (GLOBAL ONLY) -------- */
    memset(brand, 0, sizeof(brand));

    uint32_t eax, ebx, ecx, edx;
    cpuid(0x80000000, &eax, &ebx, &ecx, &edx);

    if (eax >= 0x80000004) {
        uint32_t *p = (uint32_t *)brand;
        for (uint32_t i = 0; i < 3; i++) {
            cpuid(0x80000002 + i, &p[0], &p[1], &p[2], &p[3]);
            p += 4;
        }
    } else {
        strcpy(brand, "Unknown CPU");
    }

    /* Safe copy to vendor */
    strcpy(vendor, brand);

    /* -------- MEMORY SIZE (HHDM SAFE) -------- */
    memory_amount = 0;

    if (memmap_request.response && hhdm_request.response) {
        uint64_t hhdm = hhdm_request.response->offset;

        for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
            struct limine_memmap_entry *entry =
                (struct limine_memmap_entry *)((uint64_t)
                    memmap_request.response->entries[i] + hhdm);

            if (entry->type == LIMINE_MEMMAP_USABLE) {
                memory_amount += entry->length;
            }
        }

        memory_amount /= (1024 * 1024); // bytes → MB
    }
}

void print_cpu_info(void) {
    serial_io_printf("CPU: %s\n", vendor);
    serial_io_printf("Cores: %d\n", detected);
    serial_io_printf("Memory: %llu MB\n", memory_amount);
}
