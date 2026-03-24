// ===============================
// uacpi_glue.c
// Single-core uACPI kernel glue
// ===============================
#include "kernel_api.h"

#include "../bootloader.h"
#include "../libs/string.h"
#include "../interrupts/timer.h"
#include "../gfx/serial_io.h"
#include "../mem/paging.h"
#include "../fs/pci.h"
#include "../mem/new/pmm.h"
#include "../tasks/task.h"
#include "../tasks/spinblocks.h"
#include "../interrupts/isr.h"
#include "../mem/heap.h"



#define IRQ_TO_VECTOR(irq) ((irq) + 0x20)

extern volatile struct limine_framebuffer_request framebuffer_request;
extern volatile struct limine_memmap_request memmap_request;
extern volatile struct limine_hhdm_request hhdm_request;
extern volatile struct limine_rsdp_request rsdp_request;
extern volatile struct limine_mp_request smp_request;

#define HHDM_BASE (hhdm_request.response->offset)


// ------------------------------------
// PCI device wrapper
// ------------------------------------
typedef struct {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
} uacpi_pci_device_t;

// ------------------------------------
// IO region wrapper
// ------------------------------------
typedef struct {
    uacpi_io_addr base;
    uacpi_size length;
} uacpi_io_region_t;

// ------------------------------------
// Mutex wrapper
// ------------------------------------
typedef struct { spinlock_t lock; } uacpi_mutex_t;

// ------------------------------------
// Event wrapper
// ------------------------------------
typedef struct { volatile uint32_t counter; } uacpi_event_t;

// ------------------------------------
// RSDP
// ------------------------------------
uacpi_status uacpi_kernel_get_rsdp(uacpi_phys_addr *out_rsdp_address)
{
    if (!rsdp_request.response || rsdp_request.response->address == 0)
        return UACPI_STATUS_NOT_FOUND;

    *out_rsdp_address = (uacpi_phys_addr)rsdp_request.response->address;
    return UACPI_STATUS_OK;
}
// ------------------------------------
// Physical memory mapping (HHDM)
// ------------------------------------
void *uacpi_kernel_map(uacpi_phys_addr addr, uacpi_size len) {
    uintptr_t phys = addr & ~0xFFFULL;
    uintptr_t virt = HHDM_BASE + phys;
    uintptr_t offset = addr & 0xFFFULL;

    uacpi_size total = len + offset;

    for (uacpi_size off = 0; off < total; off += 0x1000) {
        void *vaddr = (void *)(virt + off);
        void *paddr = (void *)(phys + off);
        mapPage(vaddr, paddr, PAGE_PRESENT | PAGE_WRITE | PAGE_CACHE_DISABLE);
    }
    return (void *)(virt + offset);
}
// =========================================================
// PCI device open/close
// =========================================================

uacpi_status uacpi_kernel_pci_device_open(
    uacpi_pci_address address,
    uacpi_handle *out_handle
) {
    if (!out_handle)
        return UACPI_STATUS_INVALID_ARGUMENT;

    uacpi_pci_device_t *dev = malloc(sizeof(uacpi_pci_device_t));
    if (!dev)
        return UACPI_STATUS_OUT_OF_MEMORY;

    dev->bus = address.bus;
    dev->device = address.device;
    dev->function = address.function;

    *out_handle = (uacpi_handle)dev;
    return UACPI_STATUS_OK;
}

void uacpi_kernel_pci_device_close(uacpi_handle handle) {
    if (!handle) return;
    free(handle);
}


void uacpi_kernel_unmap(void *addr, uacpi_size len) {
    serial_io_printf("\n\n I,uacpi unmap is called \n\n");
    unmapPages(addr, len);
}

// ------------------------------------
// Logging
// ------------------------------------
void uacpi_kernel_log(uacpi_log_level level, const uacpi_char *msg) {
    (void)level;
    serial_io_printf("[uACPI] %s\n", msg);
}

// ------------------------------------
// PCI handles & access
// ------------------------------------
uacpi_handle uacpi_kernel_create_pci_handle(uint8_t bus, uint8_t device, uint8_t function) {
    uacpi_pci_device_t *dev = malloc(sizeof(uacpi_pci_device_t));
    if (!dev) return NULL;
    dev->bus = bus; dev->device = device; dev->function = function;
    return (uacpi_handle)dev;
}

void uacpi_kernel_destroy_pci_handle(uacpi_handle handle) {
    if (handle) free(handle);
}

static inline uacpi_pci_device_t *get_pci_dev(uacpi_handle handle) {
    return (uacpi_pci_device_t *)handle;
}

uacpi_status uacpi_kernel_pci_read8(uacpi_handle device, uacpi_size offset, uacpi_u8 *value) {
    if (!device || !value) return UACPI_STATUS_INVALID_ARGUMENT;
    *value = pci_read_byte(get_pci_dev(device)->bus, get_pci_dev(device)->device, get_pci_dev(device)->function, (uint8_t)offset);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_read16(uacpi_handle device, uacpi_size offset, uacpi_u16 *value) {
    if (!device || !value) return UACPI_STATUS_INVALID_ARGUMENT;
    *value = pci_read_word(get_pci_dev(device)->bus, get_pci_dev(device)->device, get_pci_dev(device)->function, (uint8_t)offset);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_read32(uacpi_handle device, uacpi_size offset, uacpi_u32 *value) {
    if (!device || !value) return UACPI_STATUS_INVALID_ARGUMENT;
    *value = pci_read_dword(get_pci_dev(device)->bus, get_pci_dev(device)->device, get_pci_dev(device)->function, (uint8_t)offset);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_write8(uacpi_handle device, uacpi_size offset, uacpi_u8 value) {
    if (!device) return UACPI_STATUS_INVALID_ARGUMENT;
    pci_write_byte(get_pci_dev(device)->bus, get_pci_dev(device)->device, get_pci_dev(device)->function, (uint8_t)offset, value);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_write16(uacpi_handle device, uacpi_size offset, uacpi_u16 value) {
    if (!device) return UACPI_STATUS_INVALID_ARGUMENT;
    pci_write_word(get_pci_dev(device)->bus, get_pci_dev(device)->device, get_pci_dev(device)->function, (uint8_t)offset, value);
    return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_pci_write32(uacpi_handle device, uacpi_size offset, uacpi_u32 value) {
    if (!device) return UACPI_STATUS_INVALID_ARGUMENT;
    pci_write_dword(get_pci_dev(device)->bus, get_pci_dev(device)->device, get_pci_dev(device)->function, (uint8_t)offset, value);
    return UACPI_STATUS_OK;
}

// ------------------------------------
// System IO
// ------------------------------------
uacpi_status uacpi_kernel_io_map(uacpi_io_addr base, uacpi_size len, uacpi_handle *out_handle) {
    if (!out_handle) return UACPI_STATUS_INVALID_ARGUMENT;
    uacpi_io_region_t *region = malloc(sizeof(uacpi_io_region_t));
    if (!region) return UACPI_STATUS_OUT_OF_MEMORY;
    region->base = base; region->length = len;
    *out_handle = (uacpi_handle)region;
    return UACPI_STATUS_OK;
}

void uacpi_kernel_io_unmap(uacpi_handle handle) { if(handle) free(handle); }
static inline uacpi_io_region_t *get_io_region(uacpi_handle handle) { return (uacpi_io_region_t *)handle; }

uacpi_status uacpi_kernel_io_read8(uacpi_handle handle, uacpi_size offset, uacpi_u8 *value) {
    if (!handle || !value) return UACPI_STATUS_INVALID_ARGUMENT;
    uacpi_io_region_t *region = get_io_region(handle);
    if (offset >= region->length) return UACPI_STATUS_INVALID_ARGUMENT;
    *value = inb(region->base + offset);
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_io_read16(uacpi_handle handle, uacpi_size offset, uacpi_u16 *value) {
    if (!handle || !value) return UACPI_STATUS_INVALID_ARGUMENT;
    uacpi_io_region_t *region = get_io_region(handle);
    if (offset + 1 >= region->length) return UACPI_STATUS_INVALID_ARGUMENT;
    *value = inw(region->base + offset);
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_io_read32(uacpi_handle handle, uacpi_size offset, uacpi_u32 *value) {
    if (!handle || !value) return UACPI_STATUS_INVALID_ARGUMENT;
    uacpi_io_region_t *region = get_io_region(handle);
    if (offset + 3 >= region->length) return UACPI_STATUS_INVALID_ARGUMENT;
    *value = inl(region->base + offset);
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_io_write8(uacpi_handle handle, uacpi_size offset, uacpi_u8 value) {
    if (!handle) return UACPI_STATUS_INVALID_ARGUMENT;
    uacpi_io_region_t *region = get_io_region(handle);
    if (offset >= region->length) return UACPI_STATUS_INVALID_ARGUMENT;
    outb(region->base + offset, value);
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_io_write16(uacpi_handle handle, uacpi_size offset, uacpi_u16 value) {
    if (!handle) return UACPI_STATUS_INVALID_ARGUMENT;
    uacpi_io_region_t *region = get_io_region(handle);
    if (offset + 1 >= region->length) return UACPI_STATUS_INVALID_ARGUMENT;
    outw(region->base + offset, value);
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_io_write32(uacpi_handle handle, uacpi_size offset, uacpi_u32 value) {
    if (!handle) return UACPI_STATUS_INVALID_ARGUMENT;
    uacpi_io_region_t *region = get_io_region(handle);
    if (offset + 3 >= region->length) return UACPI_STATUS_INVALID_ARGUMENT;
    outl(region->base + offset, value);
    return UACPI_STATUS_OK;
}

// ------------------------------------
// Memory allocation
// ------------------------------------
void *uacpi_kernel_alloc(uacpi_size size) {
    if (size == 0) return NULL;
    void *ptr = malloc(size);
    if (ptr) memset(ptr, 0, size);
    return ptr;
}
void uacpi_kernel_free(void *mem) { if(mem) free(mem); }

// ------------------------------------
// Timing
// ------------------------------------
uacpi_u64 uacpi_kernel_get_nanoseconds_since_boot(void) {
    return (uacpi_u64)timer_get_ticks() * 10000000ULL;
}
void uacpi_kernel_stall(uacpi_u8 usec) { timer_sleep_ms(usec / 1000); }
void uacpi_kernel_sleep(uacpi_u64 msec) { timer_sleep_ms(msec); }

// ------------------------------------
// Mutex
// ------------------------------------
uacpi_handle uacpi_kernel_create_mutex(void) {
    uacpi_mutex_t *m = malloc(sizeof(uacpi_mutex_t));
    spinlock_init(&m->lock);
    return (uacpi_handle)m;
}
void uacpi_kernel_free_mutex(uacpi_handle h) { if(h) free(h); }
uacpi_status uacpi_kernel_acquire_mutex(uacpi_handle h, uacpi_u16 timeout) {
    if (!h) return UACPI_STATUS_INVALID_ARGUMENT;
    spin_lock(&((uacpi_mutex_t*)h)->lock);
    return UACPI_STATUS_OK;
}
void uacpi_kernel_release_mutex(uacpi_handle h) {
    if (!h) return;
    spin_unlock(&((uacpi_mutex_t*)h)->lock);
}

// ------------------------------------
// Events
// ------------------------------------
uacpi_handle uacpi_kernel_create_event(void) {
    uacpi_event_t *e = malloc(sizeof(uacpi_event_t));
    e->counter = 0;
    return (uacpi_handle)e;
}
void uacpi_kernel_free_event(uacpi_handle h) { if(h) free(h); }
uacpi_bool uacpi_kernel_wait_for_event(uacpi_handle h, uacpi_u16 timeout) {
    if(!h) return 0;
    uacpi_event_t *e = (uacpi_event_t*)h;
    uint64_t start = timer_get_ticks();
    while(e->counter == 0) {
        if(timeout != 0xFFFF && timer_get_ticks() - start > timeout) return 0;
    }
    e->counter--;
    return 1;
}
void uacpi_kernel_signal_event(uacpi_handle h) { if(h) ((uacpi_event_t*)h)->counter++; }
void uacpi_kernel_reset_event(uacpi_handle h) { if(h) ((uacpi_event_t*)h)->counter = 0; }

// ------------------------------------
// Thread info
// ------------------------------------
uacpi_thread_id uacpi_kernel_get_thread_id(void) {
    return runningTask ? runningTask->id : 0;
}

// ------------------------------------
// Spinlocks
// ------------------------------------
uacpi_handle uacpi_kernel_create_spinlock(void) {
    spinlock_t *s = malloc(sizeof(spinlock_t));
    spinlock_init(s);
    return (uacpi_handle)s;
}
void uacpi_kernel_free_spinlock(uacpi_handle h) { if(h) free(h); }
uacpi_cpu_flags uacpi_kernel_lock_spinlock(uacpi_handle h) {
    uint64_t f;
    spin_lock_irqsave((spinlock_t*)h,&f);
    return f;
}
void uacpi_kernel_unlock_spinlock(uacpi_handle h, uacpi_cpu_flags f) {
    spin_unlock_irqrestore((spinlock_t*)h,&f);
}

// ------------------------------------
// Work queue
// ------------------------------------
uacpi_status uacpi_kernel_schedule_work(uacpi_work_type type, uacpi_work_handler handler, uacpi_handle ctx) {
    if(!handler) return UACPI_STATUS_INVALID_ARGUMENT;
    handler(ctx); // single-core immediate execution
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_wait_for_work_completion(void) { return UACPI_STATUS_OK; }

// ------------------------------------
// Firmware request (stub)
// ------------------------------------
uacpi_status uacpi_kernel_handle_firmware_request(uacpi_firmware_request* req) {
    (void)req;
    serial_io_printf("[uACPI] Firmware request received\n");
    return UACPI_STATUS_OK;
}

// ------------------------------------
// Interrupts
// ------------------------------------
uacpi_status uacpi_kernel_install_interrupt_handler(uacpi_u32 irq, uacpi_interrupt_handler handler, uacpi_handle ctx, uacpi_handle *out_irq_handle) {
    if (!handler || !out_irq_handle || irq > 15)
        return UACPI_STATUS_INVALID_ARGUMENT;
    uint8_t vector = IRQ_TO_VECTOR(irq);
    register_irq_handler(vector, (void(*)(interrupt_frame_t*))handler, "uACPI");
    *out_irq_handle = (uacpi_handle)(uintptr_t)vector;
    return UACPI_STATUS_OK;
}
uacpi_status uacpi_kernel_uninstall_interrupt_handler(uacpi_interrupt_handler handler, uacpi_handle irq_handle) {
    uint8_t vector = (uint8_t)(uintptr_t)irq_handle;
    register_irq_handler(vector, NULL, "uACPI");
    return UACPI_STATUS_OK;
}