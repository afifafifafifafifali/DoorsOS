#pragma once
#include <stdint.h>
#include "../fs/ahci.h"

typedef enum {
    STORAGE_NONE,
    STORAGE_ATA,
    STORAGE_AHCI
} storage_type_t;

void storage_init(void);
storage_type_t storage_get_type(void);
int storage_read_sectors(uint64_t lba, uint32_t count, void* buffer);
int storage_write_sectors(uint64_t lba, uint32_t count, void* buffer);
void storage_set_ahci_port(HBA_PORT* port);
