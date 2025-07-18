#include "kbio.h"
#include "../libs/string.h"
#include "../mem/heap.h"

#include "../gfx/term.h"
#include "../interrupts/isr.h"
#include "../gfx/printf.h"
#include "../interrupts/multitasking.h"
#include "io.h"

#define string_copy strcpy
#define string_length strlen
#define printfch kprint

#define PS2_DATA_PORT 0x60
#define PS2_STATUS_PORT 0x64

// characterTable and shiftedCharacterTable unchanged, omitted here for brevity...


char characterTable[] = {
    0,    0,    '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
    '-',  '=',  0,    0x09, 'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
    'o',  'p',  '[',  ']',  0,    0,    'a',  's',  'd',  'f',  'g',  'h',
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

char shiftedCharacterTable[] = {
    0,    0,    '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
    '_',  '+',  0,    0x09, 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
    'O',  'P',  '{',  '}',  0,    0,    'A',  'S',  'D',  'F',  'G',  'H',
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

static bool shiftPressed = false;
static bool capsLock = false;
static string_t inputBuffer = NULL;
static size_t bufferPos = 0;
static size_t bufferSize = 0;
static bool input_finished = false;

void keyboard_irq_handler(interrupt_frame_t* frame) {
    (void)frame;

    if (input_finished) return;  // Ignore input after Enter pressed

    uint8_t scancode = inb(PS2_DATA_PORT);

    // Key release handling
    if (scancode & 0x80) {
        scancode &= 0x7F;

        if (scancode == 42 || scancode == 54) { // Shift released
            shiftPressed = false;
        }
        return;
    }

    // Key press handling
    if (scancode == 42 || scancode == 54) { // Shift pressed
        shiftPressed = true;
        return;
    }

    if (scancode == 58) { // Caps Lock toggle
        capsLock = !capsLock;
        return;
    }

    if (scancode == 0x1C) { // Enter key
        input_finished = true;
        // Null terminate buffer, do not store newline
        if (inputBuffer && bufferPos < bufferSize) {
            inputBuffer[bufferPos] = '\0';
        }
        return; // Do not echo Enter
    }
    if (scancode == 0x0E) { // Backspace
    if (bufferPos > 0) {
        bufferPos--;
        inputBuffer[bufferPos] = '\0';
        printf("\b \b");
    }
    return;
}



    char c;
    if (shiftPressed) {
        c = shiftedCharacterTable[scancode];
    } else {
        c = characterTable[scancode];
    }

    if (capsLock && c >= 'a' && c <= 'z') {
        c -= 32;
    } else if (capsLock && c >= 'A' && c <= 'Z' && !shiftPressed) {
        c += 32;
    }

    if (c) {
        if (inputBuffer && bufferPos < bufferSize - 1) {
            inputBuffer[bufferPos++] = c;
            inputBuffer[bufferPos] = '\0';
        }
        char buf[2] = {c, '\0'};
        printfch(buf); // Echo to screen
    }
}

void ps2_kbio_init(void) {
    register_irq_handler(33, keyboard_irq_handler);
    uint8_t mask = inb(0x21);
    mask &= ~(1 << 1);
    outb(0x21, mask);
}

/**
 * ps2_kbio_read:
 *  - buffStr: pointer to buffer to store input
 *  - buffSize: size of the buffer in bytes
 * 
 * Returns a newly allocated string with input on success, or NULL on failure.
 */

void reset_keyboard_input_state() {
    inputBuffer = NULL;
    bufferPos = 0;
    bufferSize = 0;
    input_finished = false;
}



string_t ps2_kbio_read(string_t buffStr, size_t buffSize) {
    if (!buffStr || buffSize == 0) return NULL;

    inputBuffer = buffStr;
    bufferPos = 0;
    bufferSize = buffSize;
    input_finished = false;
    inputBuffer[0] = '\0';

    while (!input_finished) {
        asm volatile("hlt");
    }

    string_t result = malloc(bufferPos + 1);
    if (result) {
        string_copy(result, inputBuffer);
    }

    reset_keyboard_input_state();  // <- Reset state cleanly here.

    return result;
}
