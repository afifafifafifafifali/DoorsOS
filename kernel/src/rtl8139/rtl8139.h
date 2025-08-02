#ifndef RTL8139_H
#define RTL8139_H

#include <stdint.h>
#include "../fs/pci.h"  // Assumes you have a PCI access layer
#include "../mem/heap.h" // Malloc
#include "../gfx/printf.h"// Printf
#include "../interrupts/isr.h" // void register_irq_handler(uint8_t interrupt, void (*handler) (interrupt_frame_t* frame));


// RTL8139 I/O Port Offsets
#define RTL_REG_IDR0        0x00    // MAC address (6 bytes)
#define RTL_REG_MAR0        0x08    // Multicast registers (8 bytes)
#define RTL_REG_RBSTART     0x30    // Receive buffer start address
#define RTL_REG_CMD         0x37    // Command register
#define RTL_REG_IMR         0x3C    // Interrupt mask
#define RTL_REG_ISR         0x3E    // Interrupt status
#define RTL_REG_RCR         0x44    // Receive configuration
#define RTL_REG_TCR         0x40    // Transmit configuration
#define RTL_REG_TXADDR0     0x20    // Tx descriptors
#define RTL_REG_TXSTATUS0   0x10    // Tx status registers
#define RTL_REG_CONFIG1     0x52    // Configuration register

// Command Register bits
#define RTL_CMD_RESET       0x10
#define RTL_CMD_RX_ENABLE   0x08
#define RTL_CMD_TX_ENABLE   0x04

// Receive buffer size
#define RTL_RX_BUFFER_SIZE  8192 + 16 + 1500

// IRQ buffer sizes
#define RTL_TX_BUFFER_COUNT 4

typedef struct {
    uint32_t io_base;
    uint8_t mac[6];
    uint8_t *rx_buffer;
    uint32_t rx_buffer_offset;
    uint32_t tx_cur;
} rtl8139_dev_t;

extern rtl8139_dev_t rtl_dev;

void rtl8139_init(void);
void rtl8139_send_packet(const void *data, uint32_t length);
void rtl8139_handle_interrupt(void);
//void what_is_my_ip(void);

#endif // RTL8139_H
