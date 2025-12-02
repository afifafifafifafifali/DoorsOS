#include "rtl8139.h"
#include "../fs/pci.h"
#include "../gfx/printf.h"
#include "../mem/paging.h"
#include "../mem/heap.h"
#include "../ps2/io.h"

#define RTL8139_VENDOR_ID 0x10EC
#define RTL8139_DEVICE_ID 0x8139

#define REG_MAC0        0x00
#define REG_RBSTART     0x30
#define REG_CR          0x37
#define REG_CAPR        0x38
#define REG_CBR         0x3A
#define REG_IMR         0x3C
#define REG_ISR         0x3E
#define REG_TCR         0x40
#define REG_RCR         0x44
#define REG_TSAD0       0x20
#define REG_TSD0        0x10

static uint32_t rtl8139_io_base = 0;
static uint8_t* rx_buffer = 0;
static uint8_t* tx_buffer[4] = {0};
static uint8_t current_tx = 0;
static uint8_t rtl_bus, rtl_device, rtl_function;

static int find_rtl8139(void) {
    for (int bus = 0; bus < 8; bus++) {  // Only scan first 8 buses
        for (uint8_t device = 0; device < 32; device++) {
            uint16_t vendor = pci_read_word(bus, device, 0, 0x00);
            if (vendor == 0xFFFF) continue;
            uint16_t dev_id = pci_read_word(bus, device, 0, 0x02);
            
            if (vendor == RTL8139_VENDOR_ID && dev_id == RTL8139_DEVICE_ID) {
                rtl_bus = bus;
                rtl_device = device;
                rtl_function = 0;
                return 1;
            }
        }
    }
    return 0;
}

void rtl8139_init(void) {
    printf("Initializing RTL8139 network card...\n");
    
    if (!find_rtl8139()) {
        printf("RTL8139: No device found\n");
        return;
    }
    
    printf("RTL8139 found at bus %d, device %d, function %d\n", 
           rtl_bus, rtl_device, rtl_function);
    
    // Get I/O base address
    rtl8139_io_base = pci_read_dword(rtl_bus, rtl_device, rtl_function, 0x10) & 0xFFFFFFFC;
    printf("RTL8139 I/O base: 0x%x\n", rtl8139_io_base);
    
    // Enable PCI bus mastering
    uint16_t command = pci_read_word(rtl_bus, rtl_device, rtl_function, 0x04);
    command |= 0x04; // Bus master enable
    pci_write_word(rtl_bus, rtl_device, rtl_function, 0x04, command);
    
    // Software reset with timeout
    outb(rtl8139_io_base + REG_CR, 0x10);
    int timeout = 100000;
    while ((inb(rtl8139_io_base + REG_CR) & 0x10) != 0 && --timeout > 0);
    if (timeout == 0) {
        printf("RTL8139: Reset timeout\n");
        return;
    }
    
    // Allocate receive buffer (8KB + 16 bytes)
    rx_buffer = (uint8_t*)allocator_malloc(8192 + 16);
    if (!rx_buffer) {
        printf("Failed to allocate RX buffer\n");
        return;
    }
    
    // Allocate transmit buffers
    for (int i = 0; i < 4; i++) {
        tx_buffer[i] = (uint8_t*)allocator_malloc(1792); // Max packet size
        if (!tx_buffer[i]) {
            printf("Failed to allocate TX buffer %d\n", i);
            return;
        }
    }
    
    // Set receive buffer
    outl(rtl8139_io_base + REG_RBSTART, (uint32_t)rx_buffer);
    
    // Set IMR + ISR
    outw(rtl8139_io_base + REG_IMR, 0x0005); // Enable RX OK and TX OK interrupts
    
    // Configure receive
    outl(rtl8139_io_base + REG_RCR, 0x0000000F); // Accept all packets
    
    // Configure transmit
    outl(rtl8139_io_base + REG_TCR, 0x03000000); // Normal transmit
    
    // Enable receiver and transmitter
    outb(rtl8139_io_base + REG_CR, 0x0C);
    
    // Read MAC address
    printf("MAC Address: ");
    for (int i = 0; i < 6; i++) {
        uint8_t mac_byte = inb(rtl8139_io_base + REG_MAC0 + i);
        printf("%02x", mac_byte);
        if (i < 5) printf(":");
    }
    printf("\n");
    
    printf("RTL8139 initialized successfully\n");
}

int rtl8139_send_packet(void* data, uint32_t len) {
    if (!rtl8139_io_base || len > 1792) {
        return -1;
    }
    
    // Copy data to TX buffer
    for (uint32_t i = 0; i < len; i++) {
        tx_buffer[current_tx][i] = ((uint8_t*)data)[i];
    }
    
    // Set transmit descriptor
    outl(rtl8139_io_base + REG_TSAD0 + (current_tx * 4), (uint32_t)tx_buffer[current_tx]);
    outl(rtl8139_io_base + REG_TSD0 + (current_tx * 4), len);
    
    current_tx = (current_tx + 1) % 4;
    return 0;
}
