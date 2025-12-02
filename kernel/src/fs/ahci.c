#include "ahci.h"
#include "ahci_driver.h"
#include "../gfx/printf.h"
#include "../gfx/serial_io.h"

// Wrapper functions that map to ahci_driver.c implementations

int checkType(HBA_PORT* port) {
    uint32_t ssts = port->ssts;
    uint8_t ipm = (ssts >> 8) & 0x0F;
    uint8_t det = ssts & 0x0F;
    
    if (det != HBA_PORT_DET_PRESENT) return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE) return AHCI_DEV_NULL;
    
    switch (port->sig) {
        case SATA_SIG_ATAPI: return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:  return AHCI_DEV_SEMB;
        case SATA_SIG_PM:    return AHCI_DEV_PM;
        default:             return AHCI_DEV_SATA;
    }
}

void probePort(HBA_MEM* abar) {
    uint32_t pi = abar->pi;
    for (int i = 0; i < 32; i++) {
        if (pi & 1) {
            int dt = checkType(&abar->ports[i]);
            if (dt == AHCI_DEV_SATA) {
                printf("SATA drive at port %d\n", i);
            } else if (dt == AHCI_DEV_SATAPI) {
                printf("SATAPI drive at port %d\n", i);
            }
        }
        pi >>= 1;
    }
}

void ahci_alloc_buffers(void) {
    // Stub - actual allocation done in ahci_driver.c
}
