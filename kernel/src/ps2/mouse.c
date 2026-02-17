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

    outb(0xA0, 0x20);  // first slave
outb(0x20, 0x20);  // then master

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

void mouse_install()
{
   unsigned char _status;  //unsigned char
    
    mouse_write(0xFF);
    mouse_read();

 
  //Enable the interrupts
  mouse_wait(1);
  outb(0x64, 0x20);
  mouse_wait(0);
   _status=inb(0x60);          
   _status = (_status | 2) ;
  mouse_wait(1);
  outb(0x64, 0x60);
  mouse_wait(1);
  outb(0x60, _status);
 mouse_read();
 
 
 
  //Enable the mouse
  mouse_write(0xF4);
  mouse_read();  //Acknowledge

  register_irq_handler(44,mouse_handler);

}
