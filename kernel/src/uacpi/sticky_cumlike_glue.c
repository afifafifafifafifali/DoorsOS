#include "kernel_api.h"

#include "../bootloader.h"
#include "../libs/string.h"
#include "../interrupts/timer.h"
#include "../gfx/serial_io.h"
#include "../mem/paging.h"


extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern  volatile struct limine_rsdp_request rsdp_request;
extern volatile struct limine_mp_request smp_request;
#define HHDM_BASE (hhdm_request.response->offset)

uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address)
{
    if (!rsdp_request.response || rsdp_request.response->address == 0)
        return UACPI_STATUS_NOT_FOUND;

    *out_rsdp_address = (uacpi_phys_addr)rsdp_request.response->address;
    return UACPI_STATUS_OK;
}

void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len)
{
    uintptr_t phys = addr & ~0xFFF; // page-align down
    uintptr_t virt = HHDM_BASE + phys;

    for (size_t offset = 0; offset < len + (addr & 0xFFF); offset += 0x1000) {
        void* vaddr = (void*)(virt + offset);
        void* paddr = (void*)(phys + offset);

        uint8_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_CACHE_DISABLE;
        mapPage(vaddr, paddr, flags);
    }

    return (void*)(virt + (addr & 0xFFF)); // preserve original offset
}

void uacpi_kernel_unmap(void *addr, uacpi_size len)
{
    (void)addr;
    (void)len;
}

void uacpi_kernel_log(uacpi_log_level level, const uacpi_char *msg)
{
    (void)level; // ignore level for barebones
    serial_io_printf("[uACPI] %s \n", msg);
}
