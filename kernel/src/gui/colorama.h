#pragma once
#include <stdint.h>

#define COLOR_RGB_BLACK         0x000000
#define COLOR_RGB_WHITE         0xFFFFFF
#define COLOR_RGB_RED           0xFF0000
#define COLOR_RGB_GREEN         0x00FF00
#define COLOR_RGB_BLUE          0x0000FF
#define COLOR_RGB_YELLOW        0xFFFF00
#define COLOR_RGB_CYAN          0x00FFFF
#define COLOR_RGB_MAGENTA       0xFF00FF
#define COLOR_RGB_GRAY          0x808080
#define COLOR_RGB_DARK_GRAY     0x404040
#define COLOR_RGB_ORANGE        0xFFA500
#define COLOR_RGB_PINK          0xFFC0CB
#define COLOR_RGB_BROWN         0x8B4513
#define COLOR_RGB_LIGHT_BLUE    0xADD8E6

// Helper macro to build RGB (no alpha)
#define RGB(r, g, b) ( ((uint32_t)((r) & 0xFF) << 16) | \
                       ((uint32_t)((g) & 0xFF) << 8)  | \
                       ((uint32_t)((b) & 0xFF)) )


