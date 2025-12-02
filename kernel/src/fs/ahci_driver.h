#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "ahci.h"

int ahci_init(void);
void ahci_test(void);
bool ahci_read_sectors(HBA_PORT* port, uint64_t lba, uint32_t count, void* buf);
bool ahci_write_sectors(HBA_PORT* port, uint64_t lba, uint32_t count, void* buf);
