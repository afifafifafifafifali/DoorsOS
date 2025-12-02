#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../libs/string.h"

// PS/2 keyboard driver header file

string_t ps2_kbio_read(string_t buffStr,size_t buffSize);
string_t ps2_kbio_read_enhanced(string_t buffStr, size_t buffSize, int* cursor_y);

void ps2_kbio_init(void);

// Special key codes
#define KEY_UP    0x48
#define KEY_DOWN  0x50
#define KEY_LEFT  0x4B
#define KEY_RIGHT 0x4D

