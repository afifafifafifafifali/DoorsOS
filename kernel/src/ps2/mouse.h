#pragma once
#include <stdint.h>

struct interrupt_frame_t; // Forward declaration

typedef struct {
    int16_t x, y;
    uint8_t buttons;
} mouse_state_t;

void mouse_init(void);
void mouse_handler(void);
void mouse_irq_handler(struct interrupt_frame_t* frame);
mouse_state_t* mouse_get_state(void);
void mouse_draw_cursor(void);
void mouse_update_cursor(int16_t dx, int16_t dy);