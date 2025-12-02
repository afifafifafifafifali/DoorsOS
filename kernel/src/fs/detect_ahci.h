#include "pci.h"
#include "../mem/paging.h"  // for mapPage, flushTLB, etc.
#include "ahci.h"

typedef struct {
    uint8_t bus;
    uint8_t device;
    uint8_t function;
    uint32_t abar; // AHCI Base Memory Address (BAR5)
} AHCI_Controller;

static AHCI_Controller detected_ahci = {0};
static bool ahci_found = false;

// Callback for pci_enumerate, looks for AHCI class device
static inline void ahci_pci_callback(uint8_t bus, uint8_t device, uint8_t function, pci_device_t* dev) {
    // AHCI class = 0x01 (Mass Storage), subclass = 0x06 (SATA controller), prog IF = 0x01 (AHCI 1.0)
    if (dev->class_code == 0x01 && dev->subclass == 0x06 && dev->prog_if == 0x01) {
        detected_ahci.bus = bus;
        detected_ahci.device = device;
        detected_ahci.function = function;
        detected_ahci.abar = dev->bar[5] & PCI_BAR_MEM_MASK; // BAR5
        ahci_found = true;
    }
}

bool detectAHCIDrive(AHCI_Controller* out) {
    ahci_found = false;
    pci_enumerate(ahci_pci_callback);

    if (ahci_found) {
        if (out)
            *out = detected_ahci;
        return true;
    }

    return false;
}

#define AHCI_VIRT_BASE 0x3FF7D000

volatile HBA_MEM* check_ahci_controller() {
    AHCI_Controller ahci;

    if (detectAHCIDrive(&ahci)) {
        printf("AHCI controller found at PCI %02x:%02x.%x\n", ahci.bus, ahci.device, ahci.function);
        printf("AHCI Base Address Register (ABAR): 0x%08x\n", ahci.abar);
        serial_io_printf("AHCI Base Address Register (ABAR): 0x%08x\n", ahci.abar);
        serial_io_printf("AHCI controller found at PCI %02x:%02x.%x\n", ahci.bus, ahci.device, ahci.function);
        
        return (volatile HBA_MEM*)(uintptr_t)ahci.abar;
    } else {
        printf("No AHCI controller detected on PCI bus.\n");
        return NULL;
    }
}
