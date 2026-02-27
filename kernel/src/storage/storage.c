#include "storage.h"
#include "../fs/ahci_driver.h"
#include "../fs/ata.h"
#include "../gfx/printf.h"
#include "../gfx/serial_io.h"
#include <stdbool.h>

static storage_type_t current_storage = STORAGE_NONE;
static uint8_t test_sector[512];
static HBA_PORT* ahci_port = NULL;

static bool test_read_ahci(void) {
    serial_io_printf("test_read_ahci: calling ahci_init\n");
    if (ahci_init()) {
        serial_io_printf("test_read_ahci: ahci_init returned true\n");
        return true;
    }
    serial_io_printf("test_read_ahci: ahci_init returned false\n");
    return false;
}

static bool test_read_ata(void) {
    // Quick check if ATA is present
    serial_io_printf("Testing ATA...\n");
    ata_pio_read28(0, 1, test_sector);
    if (test_sector[510] == 0x55 && test_sector[511] == 0xAA) {
        serial_io_printf("ATA: Valid boot sector found\n");
        return true;
    }
    serial_io_printf("ATA: No valid boot sector\n");
    return false;
}

void storage_init(void) {
    printf("Detecting storage...\n");
    serial_io_printf("=== Storage Detection ===\n");
    
    // Try AHCI first (for Q35)
    serial_io_printf("Trying AHCI...\n");
    if (test_read_ahci()) {
        current_storage = STORAGE_AHCI;
        printf("Using AHCI storage\n");
        serial_io_printf("Storage: AHCI\n");
        return;
    }
    
    // Try ATA if AHCI failed
    serial_io_printf("Trying ATA...\n");
    if (test_read_ata()) {
        current_storage = STORAGE_ATA;
        printf("Using ATA storage\n");
        serial_io_printf("Storage: ATA\n");
        return;
    }
    
    printf("No storage detected!\n");
    serial_io_printf("Storage: NONE\n");
}

storage_type_t storage_get_type(void) {
    return current_storage;
}

int storage_read_sectors(uint64_t lba, uint32_t count, void* buffer) {
    switch (current_storage) {
        case STORAGE_AHCI:
            if (ahci_port) {
                return ahci_read_sectors(ahci_port, lba, count, buffer);
            }
            return 0;
        case STORAGE_ATA:
            ata_pio_read28(lba, count, buffer);
            return 1;
        default:
            return 0;
    }
}

int storage_write_sectors(uint64_t lba, uint32_t count, void* buffer) {
    switch (current_storage) {
        case STORAGE_AHCI:
            if (ahci_port) {
                return ahci_write_sectors(ahci_port, lba, count, buffer);
            }
            return 0;
        case STORAGE_ATA:
            ata_pio_write48(lba, count, buffer);
            return 1;
        default:
            return 0;
    }
}

void storage_set_ahci_port(HBA_PORT* port) {
    ahci_port = port;
}
