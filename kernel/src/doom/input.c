#include "input.h"

#include "../libs/string.h"
#include "../ps2/kbio.h"
#include "../ps2/mouse.h"

static bool doom_translate_char(char c, doom_key_event_t *out) {
    if (!out) return false;

    switch (c) {
        case 0: return false;
        case '\n':
            out->key = DOOM_KEY_ENTER;
            out->pressed = true;
            return true;
        case ' ':
            out->key = DOOM_KEY_SPACE;
            out->pressed = true;
            return true;
        case 27: /* ESC */
            out->key = DOOM_KEY_ESC;
            out->pressed = true;
            return true;
        default:
            return false;
    }
}

bool doom_kbio_poll_event(doom_key_event_t *out) {
    if (!out) return false;
    memset(out, 0, sizeof(*out));

    char c = ps2_kbio_getchar_nb();
    if (c == 0) return false;

    /* Arrow keys are emitted as ANSI escape sequences: ESC [ A/B/C/D
       (see `kernel/src/snake.c`). */
    if (c == '\033') {
        char seq = ps2_kbio_getchar_nb();
        if (seq != '[') return false;
        char arrow = ps2_kbio_getchar_nb();
        out->pressed = true;
        switch (arrow) {
            case 'A': out->key = DOOM_KEY_UP; return true;
            case 'B': out->key = DOOM_KEY_DOWN; return true;
            case 'C': out->key = DOOM_KEY_RIGHT; return true;
            case 'D': out->key = DOOM_KEY_LEFT; return true;
            default: return false;
        }
    }

    return doom_translate_char(c, out);
}

bool doom_mouse_poll_event(doom_mouse_event_t *out) {
    if (!out) return false;
    memset(out, 0, sizeof(*out));

    /* Snapshot then clear edge flags (match syscall behavior for /dev/mouse). */
    mouse_state_t snap = mstate;
    mstate.left_pressed = 0;
    mstate.left_released = 0;
    mstate.right_pressed = 0;
    mstate.right_released = 0;
    mstate.mid_pressed = 0;
    mstate.mid_released = 0;

    out->dx = snap.dx;
    out->dy = snap.dy;
    out->dz = snap.dz;
    out->buttons_raw = snap.buttons_raw;
    out->left_down = snap.left_down;
    out->right_down = snap.right_down;
    out->mid_down = snap.mid_down;
    out->left_pressed = snap.left_pressed;
    out->left_released = snap.left_released;
    out->right_pressed = snap.right_pressed;
    out->right_released = snap.right_released;
    out->mid_pressed = snap.mid_pressed;
    out->mid_released = snap.mid_released;

    return true;
}

