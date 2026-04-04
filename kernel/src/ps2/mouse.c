#include "mouse.h"
#include "io.h"
#include "../interrupts/pic.h"
#include "../interrupts/idt.h"
#include "../interrupts/isr.h"
#include "../gfx/serial_io.h"

/* -----------------------------------------------------------------------
 * Legacy globals (kept for compat with any code that still reads these)
 * -----------------------------------------------------------------------*/
byte mouse_cycle = 0;
sbyte mouse_byte[4];  /* 4-byte Intellimouse packet */
sbyte mouse_x = 0;
sbyte mouse_y = 0;
byte mouse_buttons = 0;

/* -----------------------------------------------------------------------
 * New structured state
 * -----------------------------------------------------------------------*/
mouse_state_t mstate = {0};

/* -----------------------------------------------------------------------
 * Decode a single PS/2 mouse packet and update button edges
 * -----------------------------------------------------------------------*/
void mouse_decode_packet(byte status_byte, sbyte dx_byte, sbyte dy_byte, sbyte scroll_byte)
{
    /* Save old button state for edge detection */
    byte old_left  = mstate.left_down;
    byte old_right = mstate.right_down;
    byte old_mid   = mstate.mid_down;

    /* Button bits: bit 0=left, 1=right, 2=middle */
    mstate.buttons_raw = status_byte & 0x07;
    mstate.left_down   = (status_byte >> 0) & 1;
    mstate.right_down  = (status_byte >> 1) & 1;
    mstate.mid_down    = (status_byte >> 2) & 1;

    /* Edge detection */
    mstate.left_pressed   = mstate.left_down   && !old_left;
    mstate.left_released  = !mstate.left_down  &&  old_left;
    mstate.right_pressed  = mstate.right_down  && !old_right;
    mstate.right_released = !mstate.right_down &&  old_right;
    mstate.mid_pressed    = mstate.mid_down    && !old_mid;
    mstate.mid_released   = !mstate.mid_down   &&  old_mid;

    /* Movement */
    mstate.dx = dx_byte;
    mstate.dy = -dy_byte;  /* invert Y for natural scrolling */

    /* Scroll wheel — lower 4 bits are signed (-8..+7) */
    mstate.dz = (sbyte)(scroll_byte & 0x0F);
    if (mstate.dz & 0x08)
        mstate.dz |= 0xF0;  /* sign-extend from 4 bits */
}

/* -----------------------------------------------------------------------
 * Print button status — call this from the IRQ handler for debugging
 * -----------------------------------------------------------------------*/
void mouse_print_buttons(void)
{
    serial_io_printf("  [L:%c%s] [R:%c%s] [M:%c%s]",
                     mstate.left_down   ? '*' : ' ',
                     mstate.left_pressed   ? "press"   :
                     mstate.left_released  ? "release" :
                     mstate.left_down      ? "held"    : "---",

                     mstate.right_down  ? '*' : ' ',
                     mstate.right_pressed   ? "press"   :
                     mstate.right_released  ? "release" :
                     mstate.right_down      ? "held"    : "---",

                     mstate.mid_down    ? '*' : ' ',
                     mstate.mid_pressed   ? "press"   :
                     mstate.mid_released  ? "release" :
                     mstate.mid_down      ? "held"    : "---");
}

/* -----------------------------------------------------------------------
 * IRQ handler
 * -----------------------------------------------------------------------*/
void mouse_handler(interrupt_frame_t* frame)
{
    (void)frame;

    if (!(inb(0x64) & 1)) {
        send_eoi_to_irq(12);
        return;
    }

    byte status = inb(0x60);

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
            mouse_cycle++;
            break;

        case 3:
            mouse_byte[3] = (sbyte)status;

            /* Decode into structured state (Intellimouse 4-byte) */
            mouse_decode_packet(mouse_byte[0], mouse_byte[1],
                                mouse_byte[2], mouse_byte[3]);

            /* Update legacy globals (compat) */
            mouse_buttons = mouse_byte[0];
            mouse_x       = mstate.dx;
            mouse_y       = mstate.dy;

            /* Debug output — shows movement + button events + scroll */
            serial_io_printf("Mouse: X=%d Y=%d Scroll=%d Btn=0x%02x ",
                             mouse_x, mouse_y, mstate.dz, mouse_buttons);
            mouse_print_buttons();
            serial_io_printf("\n");

            mouse_cycle = 0;
            break;
    }

    send_eoi_to_irq(12);
}

/* -----------------------------------------------------------------------
 * Mouse init (unchanged — just the IRQ handler above was swapped)
 * -----------------------------------------------------------------------*/
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

    mouse_wait(1);
    outb(0x64, 0xA7);

    for (int i = 0; i < 1000; i++) io_wait();

    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    status = inb(0x60);
    serial_io_printf("[MOUSE] Initial controller status: 0x%x\n", status);

    status |=  (1 << 5);
    status &= ~(1 << 4);
    status |=  (1 << 0);

    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);

    serial_io_printf("[MOUSE] New controller status: 0x%x\n", status);

    mouse_wait(1);
    outb(0x64, 0xA8);
    serial_io_printf("[MOUSE] Auxiliary device enabled\n");

    outb(0xA1, inb(0xA1) & ~(1 << 4));
    outb(0x21, inb(0x21) & ~(1 << 2));

    serial_io_printf("[MOUSE] PIC unmasked\n");

    serial_io_printf("[MOUSE] Resetting mouse...\n");
    mouse_write(0xFF);
    ack = mouse_read();
    if (ack != 0xFA) {
        serial_io_printf("[MOUSE] Reset NOT ACKed: 0x%x\n", ack);
    } else {
        serial_io_printf("[MOUSE] Reset ACKed\n");
    }

    byte selftest = mouse_read();
    if (selftest != 0xAA) {
        serial_io_printf("[MOUSE] Self-test FAILED: 0x%x\n", selftest);
    } else {
        serial_io_printf("[MOUSE] Self-test passed\n");
    }

    byte mouse_id = mouse_read();
    serial_io_printf("[MOUSE] Mouse ID: 0x%x\n", mouse_id);

    serial_io_printf("[MOUSE] Setting defaults...\n");
    mouse_write(0xF6);
    ack = mouse_read();
    if (ack != 0xFA) {
        serial_io_printf("[MOUSE] Set defaults NOT ACKed: 0x%x\n", ack);
    }

    serial_io_printf("[MOUSE] Enabling data reporting...\n");

    /* Enable Intellimouse (scroll wheel) — magic sequence */
    serial_io_printf("[MOUSE] Enabling Intellimouse mode...\n");
    mouse_write(0xF3); ack = mouse_read();
    mouse_write(200);  ack = mouse_read();
    mouse_write(0xF3); ack = mouse_read();
    mouse_write(100);  ack = mouse_read();
    mouse_write(0xF3); ack = mouse_read();
    mouse_write(80);   ack = mouse_read();
    serial_io_printf("[MOUSE] Intellimouse sequence sent\n");

    mouse_write(0xF4);
    ack = mouse_read();
    if (ack != 0xFA) {
        serial_io_printf("[MOUSE] Enable data reporting NOT ACKed: 0x%x\n", ack);
    } else {
        serial_io_printf("[MOUSE] Data reporting enabled\n");
    }

    register_irq_handler(44, mouse_handler, "Mouse");

    serial_io_printf("[MOUSE] Mouse installed successfully!\n");
}
