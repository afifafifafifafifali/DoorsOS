#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    DOOM_KEY_NONE = 0,
    DOOM_KEY_UP,
    DOOM_KEY_DOWN,
    DOOM_KEY_LEFT,
    DOOM_KEY_RIGHT,
    DOOM_KEY_ESC,
    DOOM_KEY_ENTER,
    DOOM_KEY_SPACE,
} doom_key_t;

typedef struct {
    doom_key_t key;
    bool pressed;
} doom_key_event_t;

/* Non-blocking: returns true if an event was produced. Handles arrow keys
   using the same ANSI escape sequence style as `kernel/src/snake.c`. */
bool doom_kbio_poll_event(doom_key_event_t *out);

typedef struct {
    int dx;
    int dy;
    int dz;
    uint8_t buttons_raw;
    bool left_down;
    bool right_down;
    bool mid_down;
    bool left_pressed;
    bool left_released;
    bool right_pressed;
    bool right_released;
    bool mid_pressed;
    bool mid_released;
} doom_mouse_event_t;

/* Kernel-side non-blocking mouse snapshot (IRQ-updated). */
bool doom_mouse_poll_event(doom_mouse_event_t *out);

