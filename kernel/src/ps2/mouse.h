#pragma once

#include "../libs/utilities.h"

/* -----------------------------------------------------------------------
 * Mouse state tracking
 * -----------------------------------------------------------------------*/
typedef enum {
    MOUSE_BTN_NONE  = 0,
    MOUSE_BTN_LEFT  = (1 << 0),
    MOUSE_BTN_RIGHT = (1 << 1),
    MOUSE_BTN_MID   = (1 << 2),
} mouse_btn_t;

typedef struct {
    /* Current button bitmask (raw from packet) */
    byte        buttons_raw;

    /* Per-button state: 1 = currently held, 0 = released */
    byte        left_down   : 1;
    byte        right_down  : 1;
    byte        mid_down    : 1;

    /* Edge-detection flags (set once, cleared by consumer) */
    byte        left_pressed  : 1;   /* went 0 → 1 this packet */
    byte        left_released : 1;   /* went 1 → 0 this packet */
    byte        right_pressed : 1;
    byte        right_released: 1;
    byte        mid_pressed   : 1;
    byte        mid_released  : 1;

    /* Movement */
    sbyte       dx;
    sbyte       dy;

    /* Scroll wheel (signed — positive = up, negative = down) */
    sbyte       dz;
} mouse_state_t;

/* Global mouse state — updated by IRQ handler */
extern mouse_state_t mstate;

void mouse_install();

/* Decode a raw PS/2 mouse packet into `mstate` (4-byte Intellimouse) */
void mouse_decode_packet(byte status_byte, sbyte dx_byte, sbyte dy_byte, sbyte scroll_byte);

/* Print human-readable button status to serial (debug) */
void mouse_print_buttons(void);