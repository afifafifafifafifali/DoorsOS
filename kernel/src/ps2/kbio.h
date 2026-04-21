#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../libs/string.h"

// PS2 Port definitions
typedef enum {
    PS2_DATA_PORT    = 0x60,
    PS2_STATUS_PORT  = 0x64,
    PS2_COMMAND_PORT = 0x64,
} ps2_port_t;

// PS2 Status register bits
typedef enum {
    PS2_STATUS_OUTPUT_FULL    = 0x01,
    PS2_STATUS_INPUT_FULL     = 0x02,
    PS2_STATUS_SYSTEM_FLAG    = 0x04,
    PS2_STATUS_COMMAND_DATA   = 0x08,
    PS2_STATUS_LOCKED         = 0x10,
    PS2_STATUS_AUX_OUTPUT_FULL = 0x20,
    PS2_STATUS_TIMEOUT_ERROR  = 0x40,
    PS2_STATUS_PARITY_ERROR   = 0x80,
} ps2_status_bits_t;

// PS2 Scancodes (Set 2) - all scancodes as enums
typedef enum {
    SCAN_ESCAPE       = 0x01,
    SCAN_1            = 0x02,
    SCAN_2            = 0x03,
    SCAN_3            = 0x04,
    SCAN_4            = 0x05,
    SCAN_5            = 0x06,
    SCAN_6            = 0x07,
    SCAN_7            = 0x08,
    SCAN_8            = 0x09,
    SCAN_9            = 0x0A,
    SCAN_0            = 0x0B,
    SCAN_MINUS        = 0x0C,
    SCAN_EQUALS       = 0x0D,
    SCAN_BACKSPACE    = 0x0E,
    SCAN_TAB          = 0x0F,
    SCAN_Q            = 0x10,
    SCAN_W            = 0x11,
    SCAN_E            = 0x12,
    SCAN_R            = 0x13,
    SCAN_T            = 0x14,
    SCAN_Y            = 0x15,
    SCAN_U            = 0x16,
    SCAN_I            = 0x17,
    SCAN_O            = 0x18,
    SCAN_P            = 0x19,
    SCAN_LBRACKET     = 0x1A,
    SCAN_RBRACKET     = 0x1B,
    SCAN_ENTER        = 0x1C,
    SCAN_LCTRL        = 0x1D,
    SCAN_A            = 0x1E,
    SCAN_S            = 0x1F,
    SCAN_D            = 0x20,
    SCAN_F            = 0x21,
    SCAN_G            = 0x22,
    SCAN_H            = 0x23,
    SCAN_J            = 0x24,
    SCAN_K            = 0x25,
    SCAN_L            = 0x26,
    SCAN_SEMICOLON    = 0x27,
    SCAN_QUOTE        = 0x28,
    SCAN_GRAVE        = 0x29,
    SCAN_LSHIFT       = 0x2A,
    SCAN_BACKSLASH    = 0x2B,
    SCAN_Z            = 0x2C,
    SCAN_X            = 0x2D,
    SCAN_C            = 0x2E,
    SCAN_V            = 0x2F,
    SCAN_B            = 0x30,
    SCAN_N            = 0x31,
    SCAN_M            = 0x32,
    SCAN_COMMA        = 0x33,
    SCAN_PERIOD       = 0x34,
    SCAN_SLASH        = 0x35,
    SCAN_RSHIFT       = 0x36,
    SCAN_ASTERISK     = 0x37,
    SCAN_RALT         = 0x38,
    SCAN_SPACE        = 0x39,
    SCAN_CAPSLOCK     = 0x3A,
    SCAN_F1           = 0x3B,
    SCAN_F2           = 0x3C,
    SCAN_F3           = 0x3D,
    SCAN_F4           = 0x3E,
    SCAN_F5           = 0x3F,
    SCAN_F6           = 0x40,
    SCAN_F7           = 0x41,
    SCAN_F8           = 0x42,
    SCAN_F9           = 0x43,
    SCAN_F10          = 0x44,
    SCAN_NUMLOCK      = 0x45,
    SCAN_SCROLLLOCK   = 0x46,
    SCAN_HOME         = 0x47,
    SCAN_UP           = 0x48,
    SCAN_PAGEUP       = 0x49,
    SCAN_LEFT         = 0x4B,
    SCAN_RIGHT        = 0x4D,
    SCAN_END          = 0x4F,
    SCAN_DOWN         = 0x50,
    SCAN_PAGEDOWN     = 0x51,
    SCAN_INSERT       = 0x52,
    SCAN_DELETE       = 0x53,
    SCAN_F11          = 0x57,
    SCAN_F12          = 0x58,
    SCAN_KEY_RELEASE  = 0x80,  // Bit to OR with scancode for release
} ps2_scancode_t;

typedef enum {
    PS2_CMD_SET_SCANCODE = 0xF0,
    PS2_CMD_ENABLE       = 0xF4,
    PS2_CMD_DISABLE      = 0xF5,
    PS2_CMD_RESET        = 0xFF,
    PS2_CMD_SET_LEDS     = 0xED,
} ps2_command_t;

typedef enum {
    PS2_RESP_ACK         = 0xFA,
    PS2_RESP_RESEND      = 0xFE,
    PS2_RESP_ERROR       = 0xFC,
} ps2_response_t;

typedef enum {
    KBIO_MODE_RAW,    // No echoing, direct scancode access (for games)
    KBIO_MODE_COOKED, // Echo characters, line buffered (for shell)
} kbio_mode_t;

// Keyboard configuration
typedef enum {
    KBIO_BUFFER_SIZE     = 512,  // Increased ring buffer size
    KBIO_MIN_BUFFER_SIZE = 64,
} kbio_config_t;

typedef struct {
    uint8_t scancode;
    bool    pressed;
    bool    shift;
    bool    ctrl;
    bool    alt;
} kbio_event_t;

string_t ps2_kbio_read(string_t buffStr, size_t buffSize);
string_t ps2_kbio_read_enhanced(string_t buffStr, size_t buffSize, int* cursor_y);
void ps2_kbio_init(void);
char ps2_kbio_getchar_nb(void);
void reset_keyboard_input_state(void);
void kbio_flush_buffer(void);
void kbio_set_mode(kbio_mode_t mode);
kbio_mode_t kbio_get_mode(void);
bool kbio_get_event(kbio_event_t* event);

#define KEY_UP    SCAN_UP
#define KEY_DOWN  SCAN_DOWN
#define KEY_LEFT  SCAN_LEFT
#define KEY_RIGHT SCAN_RIGHT
