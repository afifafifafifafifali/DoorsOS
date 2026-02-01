#include "old_asf_acpi.h"
#include "bootloader.h"
#include "libs/string.h"
#include "interrupts/timer.h"
#include "gfx/serial_io.h"
#include "mem/paging.h"

// Mostly bullshit from https://forum.osdev.org/viewtopic.php?t=16990
#define sleep timer_sleep_ms
#define wrstr serial_io_printf
#define HHDM_BASE (hhdm_request.response->offset)

uint16_t SMI_CMD      = 0;
uint8_t  ACPI_ENABLE  = 0;
uint8_t  ACPI_DISABLE = 0;
uint16_t PM1a_CNT     = 0;
uint16_t PM1b_CNT     = 0;
uint16_t SLP_TYPa     = 0;
uint16_t SLP_TYPb     = 0;
uint16_t SLP_EN       = 1 << 13;
uint16_t SCI_EN       = 1;
uint8_t  PM1_CNT_LEN  = 0;

struct RSDPtr {
    uint8_t Signature[8];
    uint8_t CheckSum;
    uint8_t OemID[6];
    uint8_t Revision;
    uint32_t *RsdtAddress;
};

struct FACP {
    uint8_t Signature[4];
    uint32_t Length;
    uint8_t unneded1[32];
    uint32_t *DSDT;
    uint8_t unneded2[4];
    uint32_t *SMI_CMD;
    uint8_t ACPI_ENABLE;
    uint8_t ACPI_DISABLE;
    uint8_t unneded3[10];
    uint32_t *PM1a_CNT_BLK;
    uint32_t *PM1b_CNT_BLK;
    uint8_t unneded4[17];
    uint8_t PM1_CNT_LEN;
};

static void* map_mmio(uintptr_t phys, size_t size) {
    uintptr_t virt = HHDM_BASE + phys;

    for (size_t offset = 0; offset < size; offset += 0x1000) {
        void* vaddr = (void*)(virt + offset);
        void* paddr = (void*)(phys + offset);

        // Present + Writable + Kernel (not user) + cache disabled for MMIO
        uint8_t flags = PAGE_PRESENT | PAGE_WRITE | PAGE_CACHE_DISABLE;

        mapPage(vaddr, paddr, flags);
    }

    return (void*)virt;
}

uint32_t *acpiCheckRSDPtr(void) {
    if (!rsdp_request.response || rsdp_request.response->address == 0)
        return NULL;

    struct RSDPtr *rsdp = map_mmio((uintptr_t)rsdp_request.response->address, 0x20);

    /* checksum */
    uint8_t sum = 0;
    uint8_t *bytes = (uint8_t*)rsdp;
    for (size_t i = 0; i < sizeof(struct RSDPtr); i++)
        sum += bytes[i];
    if (sum != 0) return NULL;

    return map_mmio((uintptr_t)rsdp->RsdtAddress, 0x1000); // donate 1 page for RSDT
}

int acpiInit(void) {
    wrstr("CHECKING RSDPTR\n");
    uint32_t *rsdt = acpiCheckRSDPtr();
    if (!rsdt) {
        wrstr("ACPI: RSDP not found!\n");
        return -1;
    }

    uint32_t length = *((uint32_t*)((uint8_t*)rsdt + 4));
    uint32_t entries = (length - 36) / 4;
    uint32_t *entry_ptr = (uint32_t*)((uint8_t*)rsdt + 36);

    struct FACP *facp = NULL;
    wrstr("LOOPING RSDT ENTRIES\n");
    for (uint32_t i = 0; i < entries; i++) {
        if (!entry_ptr[i]) continue;
        struct FACP *candidate = map_mmio((uintptr_t)entry_ptr[i], 0x1000);
        if (memcmp(candidate->Signature, "FACP", 4) == 0) {
            facp = candidate;
            break;
        }
    }
    if (!facp) {
        wrstr("ACPI: FACP not found!\n");
        return -1;
    }

    wrstr("POPULATING ACPI GLOBALS\n");
    SMI_CMD      = (uint16_t)(uintptr_t)facp->SMI_CMD;
    ACPI_ENABLE  = facp->ACPI_ENABLE;
    ACPI_DISABLE = facp->ACPI_DISABLE;
    PM1a_CNT     = (uint16_t)(uintptr_t)facp->PM1a_CNT_BLK;
    PM1b_CNT     = facp->PM1b_CNT_BLK ? (uint16_t)(uintptr_t)facp->PM1b_CNT_BLK : 0;
    PM1_CNT_LEN  = facp->PM1_CNT_LEN;
    SLP_TYPa     = 0;
    SLP_TYPb     = 0;

    wrstr("ACPI globals initialized\n");
    serial_io_printf("SMI_CMD = 0x%04x\n", SMI_CMD);
serial_io_printf("ACPI_ENABLE = 0x%02x\n", ACPI_ENABLE);
serial_io_printf("PM1a_CNT = 0x%04x\n", PM1a_CNT);
serial_io_printf("PM1b_CNT = 0x%04x\n", PM1b_CNT);

    return 0;
}

/* Enable ACPI */
int acpiEnable(void) {
    if (PM1a_CNT == 0 || SMI_CMD == 0 || ACPI_ENABLE == 0) {
        wrstr("ACPI: no enable method\n");
        return -1;
    }

    if ((inw(PM1a_CNT) & SCI_EN) != 0) return 0;

    outb(SMI_CMD, ACPI_ENABLE);

    for (int i = 0; i < 300; i++) {
        if ((inw(PM1a_CNT) & SCI_EN) != 0) break;
        sleep(10);
    }

    if (PM1b_CNT != 0) {
        for (int i = 0; i < 300; i++) {
            if ((inw(PM1b_CNT) & SCI_EN) != 0) break;
            sleep(10);
        }
    }

    if ((inw(PM1a_CNT) & SCI_EN) == 0) {
        wrstr("ACPI: enable failed\n");
        return -1;
    }

    wrstr("ACPI: enabled\n");
    return 0;
}


void acpi_shutdown(void) {
    if (PM1a_CNT == 0) {
        serial_io_printf("ACPI: no shutdown method!\n");
        return;
    }

    serial_io_printf("ACPI: entering S5 (shutdown)...\n");

    uint16_t sleep_cmd = (SLP_TYPa << 10) | SLP_EN;

    outw(PM1a_CNT, sleep_cmd);
    if (PM1b_CNT != 0) {
        outw(PM1b_CNT, (SLP_TYPb << 10) | SLP_EN);
    }

   serial_io_printf("SHOULD NEVER REACH HERE\n");
   serial_io_printf("ACPI shutdown failed, halting CPU.\n");
    while (1) { asm volatile("hlt"); }
}
