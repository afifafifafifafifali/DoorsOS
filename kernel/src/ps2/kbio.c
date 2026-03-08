#include "kbio.h"
#include "../libs/string.h"
#include "../mem/heap.h"

#include "../gfx/term.h"
#include "../interrupts/isr.h"
#include "../interrupts/pic.h"
#include "../gfx/printf.h"
#include "io.h"
#include "../tasks/task.h"

#define printfch kprint

// Ring buffer for non-blocking reads (512 bytes)
static char kbio_ring_buffer[KBIO_BUFFER_SIZE];
static volatile size_t kbio_read_idx = 0;
static volatile size_t kbio_write_idx = 0;

// Keyboard state
static bool shift_pressed = false;
static bool ctrl_pressed = false;
static bool alt_pressed = false;
static bool caps_lock = false;
static bool num_lock = false;
static bool scroll_lock = false;

static string_t input_buffer = NULL;
static volatile size_t buffer_pos = 0;
static size_t buffer_size = 0;
static volatile bool input_finished = false;

static volatile kbio_mode_t keyboard_mode = KBIO_MODE_COOKED;

static uint8_t keyboard_leds = 0;

static void keyboard_update_leds(void) {
    while (inb(PS2_STATUS_PORT) & PS2_STATUS_INPUT_FULL);
    outb(PS2_DATA_PORT, PS2_CMD_SET_LEDS);
    
    while (inb(PS2_STATUS_PORT) & PS2_STATUS_INPUT_FULL);
    outb(PS2_DATA_PORT, keyboard_leds);
}

static char get_char_from_scancode(uint8_t scancode, bool shift) {
    if (scancode >= SCAN_F12) return 0;
    
    static const char char_table[] = {
        0,    0,    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
        '-',  '=',  0,    0x09, 'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
        'o',  'p',  '[',  ']', 0,    0,    'a',  's',  'd',  'f',  'g',  'h',
        'j',  'k',  'l',  ';',  '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',
        'b',  'n',  'm',  ',',  '.',  '/',  0,    '*',  0x0F, ' ',  0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0x1B, 0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0x0E, 0x1C, 0,    0,    0,
        0,    0,    0,    0,    0,    '/',  0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0,
        0,    0,    0,    0,    0,    0,    0,    0x2C,
    };
    
    static const char shift_char_table[] = {
        0,    0,    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
        '_',  '+',  0,    0x09, 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
        'O',  'P',  '{',  '}', 0,    0,    'A',  'S',  'D',  'F',  'G',  'H',
        'J',  'K',  'L',  ':',  '"',  '~',  0,    '|',  'Z',  'X',  'C',  'V',
        'B',  'N',  'M',  '<',  '>',  '?',  0,    '*',  0x0F, ' ',  0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0x1B, 0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0x0E, 0x1C, 0,    0,    0,
        0,    0,    0,    0,    0,    '?',  0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0,
        0,    0,    0,    0,    0,    0,    0,    0x2C,
    };
    
    char c = shift ? shift_char_table[scancode] : char_table[scancode];
    
    // Apply Caps Lock for letters
    if (caps_lock && c >= 'a' && c <= 'z') {
        c -= 32;
    } else if (caps_lock && c >= 'A' && c <= 'Z' && !shift) {
        c += 32;
    }
    
    return c;
}

static void kbio_ring_push(char c) {
    size_t next_write = (kbio_write_idx + 1) % KBIO_BUFFER_SIZE;
    if (next_write != kbio_read_idx) {
        kbio_ring_buffer[kbio_write_idx] = c;
        kbio_write_idx = next_write;
    } else {
        // Buffer full - drop oldest
        kbio_read_idx = (kbio_read_idx + 1) % KBIO_BUFFER_SIZE;
        kbio_ring_buffer[kbio_write_idx] = c;
        kbio_write_idx = next_write;
    }
}

static char kbio_ring_pop(void) {
    if (kbio_read_idx == kbio_write_idx) return 0;
    char c = kbio_ring_buffer[kbio_read_idx];
    kbio_read_idx = (kbio_read_idx + 1) % KBIO_BUFFER_SIZE;
    return c;
}

void keyboard_irq_handler(interrupt_frame_t* frame) {
    (void)frame;

    // Check if data is available
    if (!(inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_FULL)) {
        send_eoi_to_irq(1);
        return;
    }

    uint8_t scancode = inb(PS2_DATA_PORT);
    
    // Check for key release (bit 7 set)
    bool is_release = (scancode & SCAN_KEY_RELEASE) != 0;
    if (is_release) {
        scancode &= ~SCAN_KEY_RELEASE;
        
        switch (scancode) {
            case SCAN_LSHIFT:
            case SCAN_RSHIFT:
                shift_pressed = false;
                break;
            case SCAN_LCTRL:
                ctrl_pressed = false;
                break;
            case SCAN_RALT:
                alt_pressed = false;
                break;
        }
        send_eoi_to_irq(1);
        return;
    }

    // Handle modifier key presses
    switch (scancode) {
        case SCAN_LSHIFT:
        case SCAN_RSHIFT:
            shift_pressed = true;
            send_eoi_to_irq(1);
            return;
            
        case SCAN_LCTRL:
            ctrl_pressed = true;
            send_eoi_to_irq(1);
            return;
            
        case SCAN_RALT:
            alt_pressed = true;
            send_eoi_to_irq(1);
            return;
            
        case SCAN_CAPSLOCK:
            caps_lock = !caps_lock;
            keyboard_leds = caps_lock ? (keyboard_leds | 0x04) : (keyboard_leds & ~0x04);
            keyboard_update_leds();
            send_eoi_to_irq(1);
            return;
            
        case SCAN_NUMLOCK:
            num_lock = !num_lock;
            keyboard_leds = num_lock ? (keyboard_leds | 0x02) : (keyboard_leds & ~0x02);
            keyboard_update_leds();
            send_eoi_to_irq(1);
            return;
            
        case SCAN_SCROLLLOCK:
            scroll_lock = !scroll_lock;
            keyboard_leds = scroll_lock ? (keyboard_leds | 0x01) : (keyboard_leds & ~0x01);
            keyboard_update_leds();
            send_eoi_to_irq(1);
            return;
    }

    // Get character from scancode
    char c = get_char_from_scancode(scancode, shift_pressed);
    
    // Handle Ctrl combinations
    if (ctrl_pressed && c >= 'a' && c <= 'z') {
        c = c - 'a' + 1;
    }
    
    // Always add to ring buffer for non-blocking reads (raw)
    if (c != 0) {
        kbio_ring_push(c);
    }
    
    // Store scancode event for games
    kbio_ring_push((char)(scancode | (is_release ? 0x80 : 0)));

    // Handle cooked mode (shell input with echoing)
    if (keyboard_mode == KBIO_MODE_COOKED && input_buffer != NULL && !input_finished) {
        
        // Handle Enter key
        if (scancode == SCAN_ENTER) {
            input_finished = true;
            if (buffer_pos < buffer_size) {
                input_buffer[buffer_pos] = '\0';
            }
            printfch("\n");
            send_eoi_to_irq(1);
            return;
        }

        // Handle Backspace
        if (scancode == SCAN_BACKSPACE) {
            if (buffer_pos > 0) {
                buffer_pos--;
                input_buffer[buffer_pos] = '\0';
                printfch("\b \b");
            }
            send_eoi_to_irq(1);
            return;
        }

        // Handle Tab
        if (scancode == SCAN_TAB) {
            if (buffer_pos < buffer_size - 1) {
                input_buffer[buffer_pos++] = '\t';
                input_buffer[buffer_pos] = '\0';
                printfch("    ");
            }
            send_eoi_to_irq(1);
            return;
        }

        // Handle Escape (clear input)
        if (scancode == SCAN_ESCAPE) {
            while (buffer_pos > 0) {
                buffer_pos--;
                printfch("\b \b");
            }
            input_buffer[0] = '\0';
            send_eoi_to_irq(1);
            return;
        }

        // Handle regular character keys with echoing
        if (c != 0 && c >= 32 && c <= 126) {
            if (buffer_pos < buffer_size - 1) {
                input_buffer[buffer_pos++] = c;
                input_buffer[buffer_pos] = '\0';
                printfch((char[]){c, '\0'});
            }
        }
    }

    send_eoi_to_irq(1);
}

void ps2_kbio_init(void) {
    register_irq_handler(33, keyboard_irq_handler, "keyboard");
    
    uint8_t mask = inb(0x21);
    mask &= ~(1 << 1);
    outb(0x21, mask);
    
    keyboard_leds = 0;
    keyboard_update_leds();
    
    keyboard_mode = KBIO_MODE_COOKED;
}

void reset_keyboard_input_state(void) {
    input_buffer = NULL;
    buffer_pos = 0;
    buffer_size = 0;
    input_finished = false;
}

void kbio_set_mode(kbio_mode_t mode) {
    keyboard_mode = mode;
}

kbio_mode_t kbio_get_mode(void) {
    return keyboard_mode;
}

string_t ps2_kbio_read(string_t buffStr, size_t buffSize) {
    if (!buffStr || buffSize == 0) return NULL;

    input_buffer = buffStr;
    buffer_pos = 0;
    buffer_size = buffSize;
    input_finished = false;
    input_buffer[0] = '\0';

    while (!input_finished) {
        if (runningTask && runningTask->slice == 0) {
            yield();
        }
        for (volatile int i = 0; i < 1000; i++);
    }

    string_t result = (string_t)malloc(buffer_pos + 1);
    if (result) {
        strcpy(result, input_buffer);
    }

    reset_keyboard_input_state();
    return result;
}

string_t ps2_kbio_read_enhanced(string_t buffStr, size_t buffSize, int* cursor_y) {
    if (!buffStr || buffSize == 0) return NULL;

    size_t index = 0;
    uint8_t scancode;

    while (index < buffSize - 1) {
        while (!(inb(PS2_STATUS_PORT) & PS2_STATUS_OUTPUT_FULL)) {
            if (runningTask && runningTask->slice == 0) {
                yield();
            }
        }

        scancode = inb(PS2_DATA_PORT);

        if (scancode & SCAN_KEY_RELEASE) continue;

        switch (scancode) {
            case SCAN_UP:
                if (cursor_y && *cursor_y > 0) {
                    (*cursor_y)--;
                    printf("\033[A");
                }
                continue;
                
            case SCAN_DOWN:
                if (cursor_y) {
                    (*cursor_y)++;
                    printf("\033[B");
                }
                continue;
                
            case SCAN_LEFT:
                if (index > 0) {
                    index--;
                    printf("\b");
                }
                continue;
                
            case SCAN_BACKSPACE:
                if (index > 0) {
                    index--;
                    printf("\b \b");
                }
                continue;
                
            case SCAN_ENTER:
                buffStr[index] = '\0';
                printf("\n");
                return buffStr;
                
            default: {
                char ch = get_char_from_scancode(scancode, shift_pressed);
                if (ch != 0 && ch >= 32 && ch <= 126) {
                    buffStr[index++] = ch;
                    printf("%c", ch);
                }
                break;
            }
        }
    }

    buffStr[index] = '\0';
    return buffStr;
}

char ps2_kbio_getchar_nb(void) {
    // Raw mode - no echoing, just return character from ring buffer
    return kbio_ring_pop();
}

bool kbio_get_event(kbio_event_t* event) {
    // For game input - returns raw scancode events
    static uint8_t last_scancode = 0;
    static bool last_pressed = false;
    
    char c = kbio_ring_pop();
    if (c == 0) return false;
    
    if (c & 0x80) {
        // Release event
        event->scancode = c & 0x7F;
        event->pressed = false;
    } else if (c < 0x80) {
        // Press event
        event->scancode = c;
        event->pressed = true;
    }
    
    event->shift = shift_pressed;
    event->ctrl = ctrl_pressed;
    event->alt = alt_pressed;
    
    return true;
}
