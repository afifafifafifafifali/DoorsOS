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



void mouse_handler(interrupt_frame_t* frame)
{
    (void)frame;
    serial_io_printf("AYYYYY MOUSE\n");
    switch (mouse_cycle)
    {
        case 0:
            mouse_byte[0] = inb(0x60);
            mouse_cycle++;
            break;

        case 1:
            mouse_byte[1] = inb(0x60);
            mouse_cycle++;
            break;

        case 2:
            mouse_byte[2] = inb(0x60);

            mouse_x = mouse_byte[1];
            mouse_y = mouse_byte[2];

            serial_io_printf("Mouse: X=%d Y=%d\n", mouse_x, mouse_y);

            mouse_cycle = 0;
            break;
    }

    serial_io_printf("IRQ12 fired\n");

  //  outb(0xA0, 0x20);  // first slave
//outb(0x20, 0x20);  // then master

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

/*
void mouse_install()
{
    unsigned char status;

    // Enable auxiliary device
    mouse_wait(1);
    outb(0x64, 0xA8);

    // Read controller status
    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    status = inb(0x60);

    // Enable IRQ12
    status |= 2;

    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);

    // Reset mouse
    mouse_write(0xFF);
    byte ack = mouse_read();
    if (ack != 0xFA) serial_io_printf("Mouse reset NOT ACK: %x\n", ack);

    // Enable streaming
    mouse_write(0xF4);
    ack = mouse_read();
    if (ack != 0xFA) serial_io_printf("Mouse enable streaming NOT ACK: %x\n", ack);

    // Unmask IRQ12
    outb(0xA1, inb(0xA1) & ~(1 << 4));

    register_irq_handler(44, mouse_handler);
}*/

void mouse_install() {
    byte ack;

    
    outb(0x21, inb(0x21) & ~(1 << 2)); // master
    outb(0xA1, inb(0xA1) & ~(1 << 4)); // slave

    
    do {
        mouse_write(0xFF);  // reset
        ack = mouse_read();
    } while (ack != 0xFA);

    byte selftest = mouse_read();
    if (selftest != 0xAA)
        serial_io_printf("Mouse self-test FAILED: %x\n", selftest);

    
    do {
        mouse_write(0xF6);  // set defaults
        ack = mouse_read();
    } while (ack != 0xFA);

   
    do {
        mouse_write(0xF4);  // enable streaming
        ack = mouse_read();
    } while (ack != 0xFA);

    
    register_irq_handler(44, mouse_handler,"Mouse");
    
    outb(0xA1, inb(0xA1) & ~(1 << 4)); // Slave: IRQ 12
    outb(0x21, inb(0x21) & ~(1 << 2)); // Master: Cascade

    serial_io_printf("Mouse installed successfully!\n");
}