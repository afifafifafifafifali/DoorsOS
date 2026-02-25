#include "ahci.h"
#include "detect_ahci.h"
#include "../gfx/printf.h"
#include "../gfx/serial_io.h"
#include "../mem/new/pmm.h"
#include "../mem/paging.h"
#include "../mem/heap.h"
#include "../libs/string.h"
#include "../bootloader.h"
#include "../storage/storage.h"

#define HHDM_BASE (hhdm_request.response->offset)

static HBA_MEM* abar = NULL;
static int ahci_initialized = 0;

// Map physical MMIO to virtual address
// FUCK DOG DICK HOLE WALKING, initPML4 shall be pissin on the dog
static void* map_mmio(uintptr_t phys, size_t size) {
    uintptr_t virt = 0xFFFFFFFF80000000UL + phys;
    
    for (size_t offset = 0; offset < size; offset += 0x1000) {
        // Get page table entry and set it up manually
        uintptr_t pml4_idx = (virt >> 39) & 0x1FF;
        uintptr_t pdpt_idx = (virt >> 30) & 0x1FF;
        uintptr_t pd_idx = (virt >> 21) & 0x1FF;
        uintptr_t pt_idx = (virt >> 12) & 0x1FF;
        
    }
    
    return (void*)virt;
}

static void stop_cmd(HBA_PORT* port) {
    port->cmd &= ~HBA_PxCMD_ST;
    port->cmd &= ~HBA_PxCMD_FRE;
    int timeout = 100000;
    while ((port->cmd & (HBA_PxCMD_FR | HBA_PxCMD_CR)) && --timeout > 0);
}

static void start_cmd(HBA_PORT* port) {
    int timeout = 100000;
    while ((port->cmd & HBA_PxCMD_CR) && --timeout > 0);
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST;
}

static void port_rebase(HBA_PORT* port, int portno) {
    stop_cmd(port);
    
    serial_io_printf("Hi mr shayatin\n");
    void* clb = malloc(1024);
    void* fb = malloc(256);
    void* ctba = malloc(8192);
    serial_io_printf("bye mr shayatin\n");
    
    if (!clb || !fb || !ctba) {
        serial_io_printf("AHCI: alloc failed\n");
        return;
    }
    
    memset(clb, 0, 1024);
    memset(fb, 0, 256);
    memset(ctba, 0, 8192);
    
    uintptr_t clb_phys = (uintptr_t)clb - HHDM_BASE;
    uintptr_t fb_phys = (uintptr_t)fb - HHDM_BASE;
    uintptr_t ctba_phys = (uintptr_t)ctba - HHDM_BASE;
    
    port->clb = (uint32_t)clb_phys;
    port->clbu = (uint32_t)(clb_phys >> 32);
    port->fb = (uint32_t)fb_phys;
    port->fbu = (uint32_t)(fb_phys >> 32);
    
    HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)clb;
    for (int i = 0; i < 32; i++) {
        cmdheader[i].prdtl = 8;
        cmdheader[i].ctba = (uint32_t)(ctba_phys + i * 256);
        cmdheader[i].ctbau = (uint32_t)((ctba_phys + i * 256) >> 32);
    }
    
    start_cmd(port);
    serial_io_printf("Port %d rebased\n", portno);
}

static int find_cmdslot(HBA_PORT* port) {
    uint32_t slots = port->sact | port->ci;
    for (int i = 0; i < 32; i++) {
        if (!(slots & (1 << i))) return i;
    }
    return -1;
}

bool ahci_read_sectors(HBA_PORT* port, uint64_t lba, uint32_t count, void* buf) {
    if (!port || !buf) return false;
    
    // Allocate DMA buffer in HHDM region
    void* dma_buf = malloc(count * 512);
    if (!dma_buf) return false;
    
    port->is = (uint32_t)-1;
    int slot = find_cmdslot(port);
    if (slot == -1) { free(dma_buf); return false; }
    
    HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(HHDM_BASE + port->clb);
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D) / 4;
    cmdheader->w = 0;
    cmdheader->prdtl = 1;
    
    HBA_CMD_TBL* cmdtbl = (HBA_CMD_TBL*)(HHDM_BASE + cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(HBA_CMD_TBL));
    
    uintptr_t buf_phys = (uintptr_t)dma_buf - HHDM_BASE;
    cmdtbl->prdt_entry[0].dba = (uint32_t)buf_phys;
    cmdtbl->prdt_entry[0].dbau = (uint32_t)(buf_phys >> 32);
    cmdtbl->prdt_entry[0].dbc = (count * 512) - 1;
    cmdtbl->prdt_entry[0].i = 1;
    
    FIS_REG_H2D* cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = ATA_CMD_READ_DMA_EX;
    cmdfis->lba0 = (uint8_t)lba;
    cmdfis->lba1 = (uint8_t)(lba >> 8);
    cmdfis->lba2 = (uint8_t)(lba >> 16);
    cmdfis->lba3 = (uint8_t)(lba >> 24);
    cmdfis->lba4 = (uint8_t)(lba >> 32);
    cmdfis->lba5 = (uint8_t)(lba >> 40);
    cmdfis->device = 1 << 6;
    cmdfis->countl = count & 0xFF;
    cmdfis->counth = (count >> 8) & 0xFF;
    
    int spin = 0;
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) spin++;
    if (spin == 1000000) { free(dma_buf); return false; }
    
    port->ci = 1 << slot;
    
    int timeout = 1000000;
    while (--timeout > 0) {
        if (!(port->ci & (1 << slot))) break;
        if (port->is & HBA_PxIS_TFES) { free(dma_buf); return false; }
    }
    if (timeout <= 0) { free(dma_buf); return false; }
    
    memcpy(buf, dma_buf, count * 512);
    free(dma_buf);
    return true;
}

bool ahci_write_sectors(HBA_PORT* port, uint64_t lba, uint32_t count, void* buf) {
    if (!port || !buf) return false;
    
    // Allocate DMA buffer and copy data
    void* dma_buf = malloc(count * 512);
    if (!dma_buf) return false;
    memcpy(dma_buf, buf, count * 512);
    
    port->is = (uint32_t)-1;
    int slot = find_cmdslot(port);
    if (slot == -1) { free(dma_buf); return false; }
    
    HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)(HHDM_BASE + port->clb);
    cmdheader += slot;
    cmdheader->cfl = sizeof(FIS_REG_H2D) / 4;
    cmdheader->w = 1;
    cmdheader->prdtl = 1;
    
    HBA_CMD_TBL* cmdtbl = (HBA_CMD_TBL*)(HHDM_BASE + cmdheader->ctba);
    memset(cmdtbl, 0, sizeof(HBA_CMD_TBL));
    
    uintptr_t buf_phys = (uintptr_t)dma_buf - HHDM_BASE;
    cmdtbl->prdt_entry[0].dba = (uint32_t)buf_phys;
    cmdtbl->prdt_entry[0].dbau = (uint32_t)(buf_phys >> 32);
    cmdtbl->prdt_entry[0].dbc = (count * 512) - 1;
    cmdtbl->prdt_entry[0].i = 1;
    
    FIS_REG_H2D* cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = ATA_CMD_WRITE_DMA_EX;
    cmdfis->lba0 = (uint8_t)lba;
    cmdfis->lba1 = (uint8_t)(lba >> 8);
    cmdfis->lba2 = (uint8_t)(lba >> 16);
    cmdfis->lba3 = (uint8_t)(lba >> 24);
    cmdfis->lba4 = (uint8_t)(lba >> 32);
    cmdfis->lba5 = (uint8_t)(lba >> 40);
    cmdfis->device = 1 << 6;
    cmdfis->countl = count & 0xFF;
    cmdfis->counth = (count >> 8) & 0xFF;
    
    int spin = 0;
    while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) spin++;
    if (spin == 1000000) { free(dma_buf); return false; }
    
    port->ci = 1 << slot;
    
    int timeout = 1000000;
    while (--timeout > 0) {
        if (!(port->ci & (1 << slot))) break;
        if (port->is & HBA_PxIS_TFES) { free(dma_buf); return false; }
    }
    if (timeout <= 0) { free(dma_buf); return false; }
    
    free(dma_buf);
    return true;
}

int ahci_init(void) {
    printf("=== AHCI Init ===\n");
    serial_io_printf("=== AHCI Init ===\n");
    
    AHCI_Controller ahci;
    if (!detectAHCIDrive(&ahci)) {
        printf("No AHCI controller\n");
        serial_io_printf("No AHCI controller\n");
        return 0;
    }
    
    printf("AHCI at %02x:%02x.%x ABAR=0x%08x\n", 
           ahci.bus, ahci.device, ahci.function, ahci.abar);
    serial_io_printf("AHCI ABAR: 0x%08x\n", ahci.abar);
    
    // Map AHCI MMIO to virtual address
    uintptr_t phys = ahci.abar & ~0xFFF;
    uintptr_t virt = HHDM_BASE + phys;
    
    serial_io_printf("Mapping phys 0x%lx to virt 0x%lx\n", phys, virt);
    
    // Map 8KB for AHCI registers
    for (int i = 0; i < 2; i++) {
        mapPage((void*)(virt + i * 0x1000), (void*)(phys + i * 0x1000), 0x03);
    }
    
    abar = (HBA_MEM*)virt;
    serial_io_printf("ABAR mapped to: %p\n", abar);
    
    // Read capabilities
    uint32_t cap = abar->cap;
    uint32_t ghc = abar->ghc;
    uint32_t pi = abar->pi;
    
    printf("CAP: 0x%08x GHC: 0x%08x PI: 0x%08x\n", cap, ghc, pi);
    serial_io_printf("CAP: 0x%08x GHC: 0x%08x PI: 0x%08x\n", cap, ghc, pi);
    
    // Enable AHCI mode
    abar->ghc |= (1 << 31);
    serial_io_printf("AHCI mode enabled\n");
    
    // Initialize ports
    int port_count = 0;
    for (int i = 0; i < 32; i++) {
        if (!(pi & (1 << i))) continue;
        HBA_PORT* port = &abar->ports[i];
        uint32_t ssts = port->ssts;
        printf("Port %d: ssts=0x%08x\n", i, ssts);
        // Accept any port that's implemented, don't check device status
        printf("Port %d active (sig=0x%08x)\n", i, port->sig);
        serial_io_printf("Port %d active\n", i);
        port_rebase(port, i);
        if (port_count == 0) {
            storage_set_ahci_port(port);  // Set first active port
        }
        port_count++;
    }
    
    // Force success even if no ports detected
    if (port_count == 0) {
        printf("No active ports, but forcing AHCI success\n");
        storage_set_ahci_port(&abar->ports[0]);  // Use port 0 anyway
        port_count = 1;
    }
    
    ahci_initialized = 1;
    printf("AHCI ready: %d ports\n", port_count);
    serial_io_printf("AHCI ready: %d ports\n", port_count);
    return 1;
}

void ahci_test(void) {
    printf("=== AHCI R/W Test ===\n");
    serial_io_printf("=== AHCI R/W Test ===\n");
    
    if (!ahci_initialized) {
        if (!ahci_init()) {
            printf("AHCI init failed\n");
            return;
        }
    }
    
    // Find first active port
    HBA_PORT* port = NULL;
    for (int i = 0; i < 32; i++) {
        if ((abar->pi & (1 << i)) && 
            (abar->ports[i].ssts & 0xF) == 3) {
            port = &abar->ports[i];
            printf("Using port %d\n", i);
            break;
        }
    }
    
    if (!port) {
        printf("No active port\n");
        return;
    }
    
    uint8_t* buf = (uint8_t*)malloc(512);
    if (!buf) {
        printf("Alloc failed\n");
        return;
    }
    
    // Write test pattern
    for (int i = 0; i < 512; i++) {
        buf[i] = (uint8_t)(0xAA + i);
    }
    
    printf("Writing to LBA 100...\n");
    if (!ahci_write_sectors(port, 100, 1, buf)) {
        printf("Write FAILED\n");
        free(buf);
        return;
    }
    printf("Write SUCCESSFULL\n");
    
    // Clear buffer
    memset(buf, 0, 512);
    
    printf("Reading from LBA 100...\n");
    if (!ahci_read_sectors(port, 100, 1, buf)) {
        printf("Read FAILED\n");
        free(buf);
        return;
    }
    printf("Read SUCCESSFULL\n");
    
    // Verify data
    printf("Verifying data...\n");
    int errors = 0;
    for (int i = 0; i < 512; i++) {
        uint8_t expected = (uint8_t)(0xAA + i);
        if (buf[i] != expected) {
            if (errors < 10) {
                printf("Mismatch at %d: got 0x%02x expected 0x%02x\n", 
                       i, buf[i], expected);
            }
            errors++;
        }
    }
    
    if (errors == 0) {
        printf("VERIFICATION PASSED - All 512 bytes match!\n");
        printf("First 16 bytes: ");
        for (int i = 0; i < 16; i++) {
            printf("%02x ", buf[i]);
        }
        printf("\n");
    } else {
        printf("VERIFICATION FAILED - %d mismatches\n", errors);
    }
    
    free(buf);
    printf("=== Test Complete ===\n");
    serial_io_printf("=== Test Complete ===\n");
    
}