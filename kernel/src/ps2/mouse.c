#include "mouse.h"
#include "io.h"
#include "../interrupts/pic.h"
#include "../interrupts/idt.h"
#include "../interrupts/isr.h"
#include "../gfx/serial_io.h"

byte mouse_cycle = 0;
sbyte mouse_byte[3];
sbyte mouse_x = 0;
sbyte mouse_y = 0;
byte mouse_buttons = 0;



void mouse_handler(interrupt_frame_t* frame)
{
    (void)frame;

    serial_io_printf("[MOUSE IRQ12]\n");

    // Check if data is actually available
    if (!(inb(0x64) & 1)) {
        serial_io_printf("[MOUSE] No data ready\n");
        send_eoi_to_irq(12);
        return;
    }

    byte status = inb(0x60);
    serial_io_printf("[MOUSE] Byte %d: 0x%x\n", mouse_cycle, status);

    switch (mouse_cycle)
    {
        case 0:
            mouse_byte[0] = (sbyte)status;
            mouse_cycle++;
            break;

        case 1:
            mouse_byte[1] = (sbyte)status;
            mouse_cycle++;
            break;

        case 2:
            mouse_byte[2] = (sbyte)status;

            mouse_buttons = mouse_byte[0];
            mouse_x = mouse_byte[1];
            mouse_y = -mouse_byte[2];  // Invert Y for natural scrolling

            serial_io_printf("Mouse: X=%d Y=%d Buttons=0x%x\n", mouse_x, mouse_y, mouse_buttons);

            mouse_cycle = 0;
            break;
    }

    send_eoi_to_irq(12);
}


static inline void mouse_wait(byte type)
{
    dword timeout = 100000;

    if (type == 0)
    {
        while (timeout--)
        {
            if (inb(0x64) & 1)
                return;
        }
    }
    else
    {
        while (timeout--)
        {
            if (!(inb(0x64) & 2))
                return;
        }
    }
}

static inline void mouse_write(byte data)
{
    mouse_wait(1);
    outb(0x64, 0xD4);

    mouse_wait(1);
    outb(0x60, data);
}

byte mouse_read()
{
    mouse_wait(0);
    return inb(0x60);
}

void mouse_install() {
    byte ack;
    byte status;

    serial_io_printf("[MOUSE] Initializing PS/2 mouse...\n");

    // First, disable mouse to avoid unwanted interrupts during setup
    mouse_wait(1);
    outb(0x64, 0xA7);  // Disable auxiliary device

    // Small delay
    for (int i = 0; i < 1000; i++) io_wait();

    // Read controller status byte
    mouse_wait(1);
    outb(0x64, 0x20);  // Read controller command byte
    mouse_wait(0);
    status = inb(0x60);
    serial_io_printf("[MOUSE] Initial controller status: 0x%x\n", status);

    // Set bit 5 (1 = enable IRQ12), clear bit 4 (0 = enable aux device)
    status |=  (1 << 5);  // Enable IRQ12
    status &= ~(1 << 4);  // Enable auxiliary device (active low)
    status |=  (1 << 0);  // Bit 0 = 1 means system flag set (usually needed)

    mouse_wait(1);
    outb(0x64, 0x60);  // Write controller command byte
    mouse_wait(1);
    outb(0x60, status);

    serial_io_printf("[MOUSE] New controller status: 0x%x\n", status);

    // Enable auxiliary device (mouse)
    mouse_wait(1);
    outb(0x64, 0xA8);  // Enable auxiliary device
    serial_io_printf("[MOUSE] Auxiliary device enabled\n");

    // Unmask IRQ12 on PIC (slave PIC, IRQ4)
    outb(0xA1, inb(0xA1) & ~(1 << 4));  // Unmask IRQ12 on slave PIC
    outb(0x21, inb(0x21) & ~(1 << 2));  // Unmask IRQ2 (cascade) on master PIC

    serial_io_printf("[MOUSE] PIC unmasked\n");

    // Reset mouse - mouse will send: ACK (0xFA) + Self-test result (0xAA) + ID (0x00)
    serial_io_printf("[MOUSE] Resetting mouse...\n");
    mouse_write(0xFF);
    ack = mouse_read();
    if (ack != 0xFA) {
        serial_io_printf("[MOUSE] Reset NOT ACKed: 0x%x\n", ack);
    } else {
        serial_io_printf("[MOUSE] Reset ACKed\n");
    }

    // Read self-test result (should be 0xAA = passed)
    byte selftest = mouse_read();
    if (selftest != 0xAA) {
        serial_io_printf("[MOUSE] Self-test FAILED: 0x%x\n", selftest);
    } else {
        serial_io_printf("[MOUSE] Self-test passed\n");
    }

    // Read mouse ID (should be 0x00 for standard mouse)
    byte mouse_id = mouse_read();
    serial_io_printf("[MOUSE] Mouse ID: 0x%x\n", mouse_id);

    // Set defaults
    serial_io_printf("[MOUSE] Setting defaults...\n");
    mouse_write(0xF6);
    ack = mouse_read();
    if (ack != 0xFA) {
        serial_io_printf("[MOUSE] Set defaults NOT ACKed: 0x%x\n", ack);
    }

    // Enable mouse data reporting
    serial_io_printf("[MOUSE] Enabling data reporting...\n");
    mouse_write(0xF4);
    ack = mouse_read();
    if (ack != 0xFA) {
        serial_io_printf("[MOUSE] Enable data reporting NOT ACKed: 0x%x\n", ack);
    } else {
        serial_io_printf("[MOUSE] Data reporting enabled\n");
    }

    // Register IRQ handler for interrupt 44 (IRQ12)
    register_irq_handler(44, mouse_handler, "Mouse");

    serial_io_printf("[MOUSE] Mouse installed successfully!\n");
}