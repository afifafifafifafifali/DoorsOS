#include "ahci.h"
#include "gfx/printf.h"
#include "libs/string.h"
#include "mem/paging.h"
#include <stddef.h>
#include <stdint.h>

static void traceAHCI(const char* str) 
{
    serial_io_printf("%s", str);
}

// Check device type
int checkType(HBA_PORT* port)
{
	uint32_t ssts = port->ssts;
 
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	switch (port->sig)
	{
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
	}
}
 
void probePort(HBA_MEM *abar)
{
	// Search disk in implemented ports
	uint32_t pi = abar->pi;
    for (size_t i = 0; i < 32; i++) 
	{
		if (pi & 1)
		{
            switch (checkType(&abar->ports[i])) 
            {
                case AHCI_DEV_SATA:
                    traceAHCI("SATA drive found at port ");
					serial_io_printf("%d\n",i);
                    break;
                case AHCI_DEV_SATAPI:
                    traceAHCI("SATAPI drive found at port ");
					serial_io_printf("%d\n",i);
                    break;
                case AHCI_DEV_SEMB:
                    traceAHCI("SEMB drive found at port ");
					serial_io_printf("%d\n",i);
                    break;
                case AHCI_DEV_PM:
                    traceAHCI("PM drive found at port ");
					serial_io_printf("%d\n",i);
                    break;
			    default:
                    traceAHCI("No drive found at port ");
					serial_io_printf("%d\n",i);
            }
            
		}
		pi >>= 1;
	}
} 

// Start command engine
void startCMD(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stopCMD(HBA_PORT *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
 
	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while (true)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
}

void portRebase(HBA_PORT *port, int port_no) {
    stopCMD(port);

    uint64_t clb_phys = ahci_cmd_buf_phys + (port_no << 10);
    uint64_t fb_phys  = ahci_cmd_buf_phys + (32 << 10) + (port_no << 8);
    uint64_t ctba_phys_base = ahci_cmd_buf_phys + (40 << 10) + (port_no << 13);

    port->clb  = (uint32_t)clb_phys;
    port->clbu = (uint32_t)(clb_phys >> 32);
    port->fb   = (uint32_t)fb_phys;
    port->fbu  = (uint32_t)(fb_phys >> 32);

    memset((uint8_t*)phys_to_virt(clb_phys), 0, 0x400);
    memset((uint8_t*)phys_to_virt(fb_phys), 0, 0x100);

    HBA_CMD_HEADER* cmd_header = (HBA_CMD_HEADER*)phys_to_virt(clb_phys);
    for (int i = 0; i < 32; i++) {
        cmd_header[i].prdtl = 8;

        uint64_t ctba_phys = ctba_phys_base + (i << 8);
        cmd_header[i].ctba  = (uint32_t)ctba_phys;
        cmd_header[i].ctbau = (uint32_t)(ctba_phys >> 32);

        memset((uint8_t*)phys_to_virt(ctba_phys), 0, 0x100);
    }

    startCMD(port);
}

// Find a free command list slot
int findCMDSlot(HBA_PORT* port, size_t cmd_slots)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = port->sact | port->ci;
    for (uint32_t i = 0; i < cmd_slots; i++)
	{
		if (!(slots & 1))
			return i;
		slots >>= 1;
	}
	traceAHCI("Cannot find free command list entry\n");
	return -1;
}

static bool runCommand(FIS_TYPE type, uint8_t write, HBA_PORT *port,
                       uint32_t start_l, uint32_t start_h, uint32_t count, uint16_t* buf)
{
    port->is = (uint32_t)-1; // clear pending interrupts
    int slot = findCMDSlot(port, 32);
    if (slot == -1) return false;

    // Map command header
    HBA_CMD_HEADER* cmd_header = (HBA_CMD_HEADER*)phys_to_virt(
        ((uint64_t)port->clbu << 32) | port->clb
    );
    cmd_header += slot;

    cmd_header->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);
    cmd_header->w = write;
    cmd_header->prdtl = (uint16_t)((count-1)/16 + 1);

    // Map command table
    uint64_t ctba_phys = ((uint64_t)cmd_header->ctbau << 32) | cmd_header->ctba;
    HBA_CMD_TBL* cmd_tbl = (HBA_CMD_TBL*)phys_to_virt(ctba_phys);
    memset(cmd_tbl, 0, sizeof(HBA_CMD_TBL) + (cmd_header->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

    // Fill PRDT
    uint16_t i;
    for (i = 0; i < cmd_header->prdtl-1; i++) {
        cmd_tbl->prdt_entry[i].dba = (uint32_t)virt_to_phys(buf);
        cmd_tbl->prdt_entry[i].dbc = 8*1024-1;
        cmd_tbl->prdt_entry[i].i = 1;
        buf += 4096; // 4K words = 8 KB
        count -= 16;
    }
    cmd_tbl->prdt_entry[i].dba = (uint32_t)virt_to_phys(buf);
    cmd_tbl->prdt_entry[i].dbc = (count << 9) - 1;
    cmd_tbl->prdt_entry[i].i = 1;

    // Setup FIS
    FIS_REG_H2D* cmd_fis = (FIS_REG_H2D*)(&cmd_tbl->cfis);
    cmd_fis->fis_type = FIS_TYPE_REG_H2D;
    cmd_fis->c = 1;
    cmd_fis->command = type;
    cmd_fis->lba0 = (uint8_t)start_l;
    cmd_fis->lba1 = (uint8_t)(start_l>>8);
    cmd_fis->lba2 = (uint8_t)(start_l>>16);
    cmd_fis->device = 1<<6;
    cmd_fis->lba3 = (uint8_t)(start_l>>24);
    cmd_fis->lba4 = (uint8_t)start_h;
    cmd_fis->lba5 = (uint8_t)(start_h>>8);
    cmd_fis->countl = count & 0xFF;
    cmd_fis->counth = (count>>8) & 0xFF;

    // Wait until port ready
    int spin = 0;
    while ((port->tfd & (ATA_DEV_BUSY|ATA_DEV_DRQ)) && spin < 1000000) spin++;
    if (spin == 1000000) { traceAHCI("Port hung\n"); return false; }

    // Issue command
    port->ci = 1<<slot;

    while (true) {
        if (!(port->ci & (1<<slot))) break;
        if (port->is & HBA_PxIS_TFES) { traceAHCI("Read/write error\n"); return false; }
    }

    if (port->is & HBA_PxIS_TFES) { traceAHCI("Read/write error\n"); return false; }

    return true;
}


inline bool ahci_read(HBA_PORT* port, uint32_t start_l, uint32_t start_h, uint32_t count, uint16_t* buf) {
    return runCommand(ATA_CMD_READ_DMA_EX, 0, port, start_l, start_h, count, buf);
}

inline bool ahci_write(HBA_PORT* port, uint32_t start_l, uint32_t start_h, uint32_t count, uint16_t* buf) {
    return runCommand(ATA_CMD_WRITE_DMA_EX, 1, port, start_l, start_h, count, buf);
}