#include "rtl8139.h"
#include "../ps2/io.h"
#include "../libs/string.h"
#include "../gfx/serial_io.h"
#include "../mem/heap.h"
#include "../bootloader.h"
#include <stddef.h>

rtl8139_dev_t rtl_dev;
static uint8_t rtl8139_found = 0;

extern volatile struct limine_hhdm_request hhdm_request;
//extern struct limine_hhdm_request hhdm_request; // For physical mapping

// PCI detection callback
static void rtl8139_pci_callback(uint8_t bus, uint8_t device, uint8_t function, pci_device_t* dev) {
    if (dev->vendor_id == 0x10EC && dev->device_id == 0x8139) {
        rtl8139_found = 1;

        // Enable I/O space + Bus mastering
        uint16_t cmd = pci_read_word(bus, device, function, PCI_COMMAND_OFFSET);
        cmd |= 0x0005;  // I/O + Bus Master
        pci_write_word(bus, device, function, PCI_COMMAND_OFFSET, cmd);

        // Save IO base
        rtl_dev.io_base = dev->bar[0] & ~0x3;

        // Reset NIC
        outb(RTL_CMD_RESET, rtl_dev.io_base + RTL_REG_CMD);
        while (inb(rtl_dev.io_base + RTL_REG_CMD) & RTL_CMD_RESET);

        // Read MAC address
        for (int i = 0; i < 6; i++)
            rtl_dev.mac[i] = inb(rtl_dev.io_base + RTL_REG_IDR0 + i);

        printf("RTL8139 found at IO 0x%x, MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
               rtl_dev.io_base,
               rtl_dev.mac[0], rtl_dev.mac[1], rtl_dev.mac[2],
               rtl_dev.mac[3], rtl_dev.mac[4], rtl_dev.mac[5]);
    }
}

void rtl8139_init(void) {
    pci_enumerate(rtl8139_pci_callback);
    if (!rtl8139_found) {
        printf("No RTL8139 found\n");
        return;
    }

    uint32_t io = rtl_dev.io_base;
    printf("Resetting RTL8139...\n");
    outb(RTL_CMD_RESET, io + RTL_REG_CMD);
    while (inb(io + RTL_REG_CMD) & RTL_CMD_RESET);

    printf("MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           rtl_dev.mac[0], rtl_dev.mac[1], rtl_dev.mac[2],
           rtl_dev.mac[3], rtl_dev.mac[4], rtl_dev.mac[5]);

    printf("RTL8139 basic init OK (no RX/TX enabled).\n");
}

// Send a raw packet (polling mode)
void rtl8139_send_packet(const void* data, uint32_t length) {
    if (length > 1792) {
        printf("[rtl8139] Packet too large.\n");
        return;
    }

    uint32_t io = rtl_dev.io_base;
    int cur = rtl_dev.tx_cur;

    // Copy packet to buffer
    void* tx_buf = k_malloc(length);
    memcpy(tx_buf, data, length);
    uint32_t phys_tx = (uint32_t)tx_buf - hhdm_request.response->offset;

    // Write TX address and length
    outl(phys_tx, io + RTL_REG_TXADDR0 + cur * 4);
    outl(length, io + RTL_REG_TXSTATUS0 + cur * 4);

    // Wait for TX complete
    while (!(inl(io + RTL_REG_TXSTATUS0 + cur * 4) & (1 << 15)));

    printf("[rtl8139] Packet sent (length=%u).\n", length);

    rtl_dev.tx_cur = (cur + 1) % RTL_TX_BUFFER_COUNT;
}

// Poll RX buffer for incoming packets
void rtl8139_poll_rx(void) {
    uint32_t io = rtl_dev.io_base;
    uint16_t isr = inw(io + RTL_REG_ISR);

    if (isr & 0x01) { // RX OK
        printf("[rtl8139] Packet received!\n");

        // Acknowledge RX
        outw(0x01, io + RTL_REG_ISR);

        // TODO: Parse packet from rx_buffer[rx_buffer_offset]
        // For now, just log packet detection.
    }
}
