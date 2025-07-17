#include "pit.h"
#include "pic.h"
#include "../ps2/io.h"
#include "isr.h"
#include "../gfx/term.h"
#include "../gfx/serial_io.h"

static uint32_t frequency = 1193180;

static void pit_callback(interrupt_frame_t* frame) {
    serial_io_printf(".");
    send_eoi_to_irq(0);
}

void init_pit(uint32_t new_frequency) {
    // Firstly, register our timer callback.
    kprint("Initializing timer\n");
    register_irq_handler(32, &pit_callback);

    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.
    uint32_t divisor = 1193180 / new_frequency;
    frequency = new_frequency;

    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor>>8) & 0xFF);

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
    kprint("Timer initialized\n");
    clear_mask_for_irq(0);
}

uint32_t pit_sleep(uint64_t milleseconds) {
    uint32_t ticks = 0;

    while (ticks < (frequency / 1000 * milleseconds)) {
        ticks++;
        __asm__ ("hlt");
    }
    return ticks;
}