#include "mouse.h"
#include "../gfx/printf.h"
#include "../gfx/term.h"
#include "../interrupts/isr.h"
#include "io.h"

static mouse_state_t mouse_state = {400, 300, 0}; // Start in center
static uint8_t mouse_cycle = 0;
static int8_t mouse_packet[3];

// Simple white cursor pattern (11x16)
static const char cursor_pattern[16][12] = {
    "X          ",
    "XX         ",
    "X.X        ",
    "X..X       ",
    "X...X      ",
    "X....X     ",
    "X.....X    ",
    "X......X   ",
    "X.......X  ",
    "X........X ",
    "X.....XXXXX",
    "X..X...X   ",
    "X.X X..X   ",
    "XX  X..X   ",
    "X    X.X   ",
    "     XX    "
};

void mouse_draw_cursor(void) {
    // Simple white X cursor
    kprint("X");
}

void mouse_update_cursor(int16_t dx, int16_t dy) {
    // Update position
    mouse_state.x += dx;
    mouse_state.y += dy;
    
    // Clamp to screen bounds
    if (mouse_state.x < 0) mouse_state.x = 0;
    if (mouse_state.y < 0) mouse_state.y = 0;
    if (mouse_state.x > 800) mouse_state.x = 800;
    if (mouse_state.y > 250) mouse_state.y = 250;
    
    // Show cursor movement
    kprint("X");
}

void mouse_init(void) {
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    uint8_t status = inb(0x60) | 2;
    outb(0x64, 0x60);
    outb(0x60, status);
    
    outb(0x64, 0xD4);
    outb(0x60, 0xF6);
    inb(0x60);
    
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
    inb(0x60);
    
    printf("Mouse initialized\n");
}

void mouse_handler(void) {
    uint8_t data = inb(0x60);
    
    switch (mouse_cycle) {
        case 0:
            mouse_packet[0] = data;
            if (!(data & 0x08)) return;
            mouse_cycle++;
            break;
        case 1:
            mouse_packet[1] = data;
            mouse_cycle++;
            break;
        case 2:
            mouse_packet[2] = data;
            mouse_cycle = 0;
            
            mouse_state.buttons = mouse_packet[0] & 0x07;
            
            // Update cursor with movement
            int16_t dx = mouse_packet[1];
            int16_t dy = -mouse_packet[2]; // Invert Y
            
            if (dx != 0 || dy != 0) {
                mouse_update_cursor(dx / 4, dy / 4); // Scale down movement
            }
            
            break;
    }
}

mouse_state_t* mouse_get_state(void) {
    return &mouse_state;
}

void mouse_irq_handler(interrupt_frame_t* frame) {
    (void)frame; // Unused parameter
    mouse_handler();
}