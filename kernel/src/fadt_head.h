#pragma once

#include "uacpi/acpi.h"
#include "uacpi/tables.h"
#include "uacpi/uacpi.h"

extern struct acpi_fadt *fadt;

static uint32_t pm1a_cnt_blk;
static uint8_t s4bios_req; 