#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include "../ps2/io.h"
#include "../gfx/serial_io.h"
// PCI config space I/O ports
#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// PCI config address construction
#define PCI_ENABLE_BIT     0x80000000

// PCI device limits
#define PCI_MAX_BUS        256
#define PCI_MAX_DEVICES    32
#define PCI_MAX_FUNCTIONS  8

// PCI header offsets
#define PCI_VENDOR_ID_OFFSET      0x00
#define PCI_DEVICE_ID_OFFSET      0x02
#define PCI_COMMAND_OFFSET        0x04
#define PCI_STATUS_OFFSET         0x06
#define PCI_REVISION_ID_OFFSET    0x08
#define PCI_PROG_IF_OFFSET        0x09
#define PCI_SUBCLASS_OFFSET       0x0A
#define PCI_CLASS_OFFSET          0x0B
#define PCI_HEADER_TYPE_OFFSET    0x0E
#define PCI_BAR0_OFFSET           0x10
#define PCI_CAP_PTR_OFFSET        0x34

// PCI header type bits
#define PCI_HEADER_TYPE_MULTI_FUNCTION 0x80

// PCI class codes
#define PCI_CLASS_BRIDGE          0x06
#define PCI_SUBCLASS_PCI_TO_PCI   0x04

// PCI BAR types
#define PCI_BAR_IO_MASK           0xFFFFFFFC
#define PCI_BAR_MEM_MASK          0xFFFFFFF0

// PCI config access
static inline uint32_t pci_config_address(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    return (uint32_t)(
        PCI_ENABLE_BIT |
        ((uint32_t)bus << 16) |
        ((uint32_t)device << 11) |
        ((uint32_t)function << 8) |
        (offset & 0xFC)
    );
}

// These must be implemented for your platform


// Read 32/16/8 bits from PCI config space
static inline uint32_t pci_read_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    outl(PCI_CONFIG_ADDRESS, pci_config_address(bus, device, function, offset));
    return inl(PCI_CONFIG_DATA);
}

static inline uint16_t pci_read_word(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t val = pci_read_dword(bus, device, function, offset & 0xFC);
    return (uint16_t)((val >> ((offset & 2) * 8)) & 0xFFFF);
}

static inline uint8_t pci_read_byte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t val = pci_read_dword(bus, device, function, offset & 0xFC);
    return (uint8_t)((val >> ((offset & 3) * 8)) & 0xFF);
}

// Write 32/16/8 bits to PCI config space
static inline void pci_write_dword(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    outl(PCI_CONFIG_ADDRESS, pci_config_address(bus, device, function, offset));
    outl(PCI_CONFIG_DATA, value);
}

static inline void pci_write_word(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t value) {
    uint32_t old = pci_read_dword(bus, device, function, offset & 0xFC);
    uint32_t shift = (offset & 2) * 8;
    uint32_t mask = 0xFFFF << shift;
    uint32_t newval = (old & ~mask) | ((uint32_t)value << shift);
    pci_write_dword(bus, device, function, offset & 0xFC, newval);
}

static inline void pci_write_byte(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint8_t value) {
    uint32_t old = pci_read_dword(bus, device, function, offset & 0xFC);
    uint32_t shift = (offset & 3) * 8;
    uint32_t mask = 0xFF << shift;
    uint32_t newval = (old & ~mask) | ((uint32_t)value << shift);
    pci_write_dword(bus, device, function, offset & 0xFC, newval);
}

// PCI device structure (header type 0)
typedef struct {
    uint16_t vendor_id;
    uint16_t device_id;
    uint16_t command;
    uint16_t status;
    uint8_t revision_id;
    uint8_t prog_if;
    uint8_t subclass;
    uint8_t class_code;
    uint8_t cache_line_size;
    uint8_t latency_timer;
    uint8_t header_type;
    uint8_t bist;
    uint32_t bar[6];
    uint32_t cardbus_cis_ptr;
    uint16_t subsystem_vendor_id;
    uint16_t subsystem_id;
    uint32_t expansion_rom_base;
    uint8_t capabilities_ptr;
    uint8_t reserved1[3];
    uint32_t reserved2;
    uint8_t interrupt_line;
    uint8_t interrupt_pin;
    uint8_t min_grant;
    uint8_t max_latency;
} pci_device_t;

// Helper to fill pci_device_t from config space
static inline void pci_read_device(uint8_t bus, uint8_t device, uint8_t function, pci_device_t* dev) {
    dev->vendor_id = pci_read_word(bus, device, function, PCI_VENDOR_ID_OFFSET);
    dev->device_id = pci_read_word(bus, device, function, PCI_DEVICE_ID_OFFSET);
    dev->command = pci_read_word(bus, device, function, PCI_COMMAND_OFFSET);
    dev->status = pci_read_word(bus, device, function, PCI_STATUS_OFFSET);
    dev->revision_id = pci_read_byte(bus, device, function, PCI_REVISION_ID_OFFSET);
    dev->prog_if = pci_read_byte(bus, device, function, PCI_PROG_IF_OFFSET);
    dev->subclass = pci_read_byte(bus, device, function, PCI_SUBCLASS_OFFSET);
    dev->class_code = pci_read_byte(bus, device, function, PCI_CLASS_OFFSET);
    dev->cache_line_size = pci_read_byte(bus, device, function, 0x0C);
    dev->latency_timer = pci_read_byte(bus, device, function, 0x0D);
    dev->header_type = pci_read_byte(bus, device, function, PCI_HEADER_TYPE_OFFSET);
    dev->bist = pci_read_byte(bus, device, function, 0x0F);
    for (int i = 0; i < 6; ++i)
        dev->bar[i] = pci_read_dword(bus, device, function, PCI_BAR0_OFFSET + i * 4);
    dev->cardbus_cis_ptr = pci_read_dword(bus, device, function, 0x28);
    dev->subsystem_vendor_id = pci_read_word(bus, device, function, 0x2C);
    dev->subsystem_id = pci_read_word(bus, device, function, 0x2E);
    dev->expansion_rom_base = pci_read_dword(bus, device, function, 0x30);
    dev->capabilities_ptr = pci_read_byte(bus, device, function, PCI_CAP_PTR_OFFSET);
    dev->interrupt_line = pci_read_byte(bus, device, function, 0x3C);
    dev->interrupt_pin = pci_read_byte(bus, device, function, 0x3D);
    dev->min_grant = pci_read_byte(bus, device, function, 0x3E);
    dev->max_latency = pci_read_byte(bus, device, function, 0x3F);
}

// PCI enumeration helpers
static inline uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t function);
static inline uint8_t pci_get_header_type(uint8_t bus, uint8_t device, uint8_t function);
static inline void pci_enumerate(void (*callback)(uint8_t, uint8_t, uint8_t, pci_device_t*));

// Example: get vendor ID
static inline uint16_t pci_get_vendor_id(uint8_t bus, uint8_t device, uint8_t function) {
    return pci_read_word(bus, device, function, PCI_VENDOR_ID_OFFSET);
}

// Example: get header type
static inline uint8_t pci_get_header_type(uint8_t bus, uint8_t device, uint8_t function) {
    return pci_read_byte(bus, device, function, PCI_HEADER_TYPE_OFFSET);
}

// PCI enumeration (recursive, calls callback for each function)
static inline void pci_check_function(uint8_t bus, uint8_t device, uint8_t function, void (*callback)(uint8_t, uint8_t, uint8_t, pci_device_t*)) {
    pci_device_t dev;
    pci_read_device(bus, device, function, &dev);
    if (dev.vendor_id == 0xFFFF) return;
    if (callback) callback(bus, device, function, &dev);

    // If this is a PCI-to-PCI bridge, recurse
    if (dev.class_code == PCI_CLASS_BRIDGE && dev.subclass == PCI_SUBCLASS_PCI_TO_PCI) {
        uint8_t secondary_bus = pci_read_byte(bus, device, function, 0x19);
        for (uint8_t d = 0; d < PCI_MAX_DEVICES; ++d)
            for (uint8_t f = 0; f < PCI_MAX_FUNCTIONS; ++f)
                if (pci_get_vendor_id(secondary_bus, d, f) != 0xFFFF)
                    pci_check_function(secondary_bus, d, f, callback);
    }
}

static inline void pci_check_device(uint8_t bus, uint8_t device, void (*callback)(uint8_t, uint8_t, uint8_t, pci_device_t*)) {
    uint16_t vendor = pci_get_vendor_id(bus, device, 0);
    if (vendor == 0xFFFF) return;
    pci_check_function(bus, device, 0, callback);

    uint8_t header_type = pci_get_header_type(bus, device, 0);
    if (header_type & PCI_HEADER_TYPE_MULTI_FUNCTION) {
        for (uint8_t function = 1; function < PCI_MAX_FUNCTIONS; ++function) {
            if (pci_get_vendor_id(bus, device, function) != 0xFFFF)
                pci_check_function(bus, device, function, callback);
        }
    }
}

static inline void pci_enumerate(void (*callback)(uint8_t, uint8_t, uint8_t, pci_device_t*)) {
    uint8_t header_type = pci_get_header_type(0, 0, 0);
    if (!(header_type & PCI_HEADER_TYPE_MULTI_FUNCTION)) {
        for (uint8_t device = 0; device < PCI_MAX_DEVICES; ++device)
            pci_check_device(0, device, callback);
    } else {
        for (uint8_t function = 0; function < PCI_MAX_FUNCTIONS; ++function) {
            if (pci_get_vendor_id(0, 0, function) != 0xFFFF) {
                for (uint8_t device = 0; device < PCI_MAX_DEVICES; ++device)
                    pci_check_device(function, device, callback);
            }
        }
    }
}

#include "../gfx/printf.h" // or whatever you're using for printf
#include "../gfx/serial_io.h"
static inline void lspci_device(uint8_t bus, uint8_t device, uint8_t function, pci_device_t* dev) {
    serial_io_printf("PCI %02x:%02x.%x  Vendor: %04x  Device: %04x  Class: %02x  Subclass: %02x  ProgIF: %02x\n",
        bus, device, function,
        dev->vendor_id,
        dev->device_id,
        dev->class_code,
        dev->subclass,
        dev->prog_if
    );
    serial_io_printf("PCI %02x:%02x.%x  Vendor: %04x  Device: %04x  Class: %02x  Subclass: %02x  ProgIF: %02x\n",
        bus, device, function,
        dev->vendor_id,
        dev->device_id,
        dev->class_code,
        dev->subclass,
        dev->prog_if
    );
}

static inline void lspci(void) {
    printf("Scanning PCI bus...\n");
    pci_enumerate(lspci_device);
}

#endif // PCI_H