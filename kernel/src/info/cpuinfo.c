#include "cpuinfo.h"
#include "../bootloader.h"
#include "../gfx/serial_io.h"
#include "../libs/string.h"
#include "../gfx/printf.h"
#include <stdint.h>

extern volatile struct limine_mp_request smp_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;

char vendor[100] = {0};
char brand[49] = {0}; // global to avoid stack issues
int detected = 0;
uint64_t memory_amount = 0;

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
    if (!smp_request.response) {
        detected = 1; // fallback: BSP only
    } else {
        detected = smp_request.response->cpu_count;
    }

    serial_io_printf("MEMSETTING\n");
    memset(brand, 0, sizeof(brand));
    serial_io_printf("INITING EAX\n");
    uint32_t eax, ebx, ecx, edx;
    serial_io_printf("EXECUTION OF CPUID \n");
    cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
    serial_io_printf("EXECUTION OF CPUID edn \n");
    serial_io_printf("Copypasintg \n");
    if (eax >= 0x80000004) {
        uint32_t *p = (uint32_t *)brand;
        for (uint32_t i = 0; i < 3; i++) {
            cpuid(0x80000002 + i, &p[0], &p[1], &p[2], &p[3]);
            p += 4;
        }
    } else {
        strcpy(brand, "Adolf Hitler National CPU Company ak47 @ 67Ghz");
    }

    serial_io_printf("SAFE COpy\n");
    strcpy(vendor, brand);

    serial_io_printf("%s \n",brand);

    serial_io_printf("memsizen");
   memory_amount = 0;

if (memmap_request.response && hhdm_request.response) {
    uintptr_t hhdm = hhdm_request.response->offset;

    for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
        serial_io_printf("RAW MEAT\n");
        struct limine_memmap_entry *entry_ptr = memmap_request.response->entries[i];

        serial_io_printf("virt addr\n");
        struct limine_memmap_entry *entry = (struct limine_memmap_entry *)(
            ((uintptr_t)entry_ptr < 0x100000000ULL) ? ((uintptr_t)entry_ptr + hhdm) : (uintptr_t)entry_ptr
        );

        serial_io_printf("SUMMM\n");
        if ((uintptr_t)entry < 0xFFFF800000000000 || (uintptr_t)entry > 0xFFFFFFFFFFFFFFFF)
            continue;

        if (entry->type == LIMINE_MEMMAP_USABLE) {
            memory_amount += entry->length;
        }
    }

    memory_amount /= (1024 * 1024); // convert bytes -> MB
    if(memory_amount < 890){
        serial_io_printf("HALT: Those MF AI ate too much RAM. Please feed kernel more memory.(minimum 890MB for stability)\n");
        printf("HALT: Those MF AI ate too much RAM. Please feed kernel more memory.(minimum 890MB for stability)\n");
        while(1){
            asm("hlt");
        }
    }
    serial_io_printf("%llu MB of memory YUM\n",memory_amount);
}

}

void print_cpu_info(void) {
    serial_io_printf("CPU: %s\n", vendor);
    serial_io_printf("Cores: %d\n", detected);
    serial_io_printf("Memory: %llu MB\n", memory_amount);
}
