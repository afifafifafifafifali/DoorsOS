#include "windows.h"
#include <stdint.h>
#include <stdbool.h>
#include "../bootloader.h"
#include "../gfx/printf.h"
#include "../gfx/term.h"

extern volatile struct limine_framebuffer_request framebuffer_request;
static struct limine_framebuffer* fb = NULL;



// Map simple 0–15 color indexes to ANSI codes (standard + bright)

static const char* ansi_fg_codes[] = {
    "30", "31", "32", "33", "34", "35", "36", "37",     // 0–7 normal
    "90", "91", "92", "93", "94", "95", "96", "97"      // 8–15 bright
};

static const char* ansi_bg_codes[] = {
    "40", "41", "42", "43", "44", "45", "46", "47",     // 0–7 normal
    "100", "101", "102", "103", "104", "105", "106", "107" // 8–15 bright
};

// Print text with ANSI color (4-bit or truecolor based on mode)
void kprint_color(const char* text,
                  uint32_t fg_color, bool fg_truecolor,
                  uint32_t bg_color, bool bg_truecolor) {
    char buf[1024];

    if (fg_truecolor && bg_truecolor) {
        // 24-bit foreground and background
        snprintf(buf, sizeof(buf),
                 "\x1b[38;2;%u;%u;%um\x1b[48;2;%u;%u;%um%s\x1b[0m",
                 (fg_color >> 16) & 0xFF, (fg_color >> 8) & 0xFF, fg_color & 0xFF,
                 (bg_color >> 16) & 0xFF, (bg_color >> 8) & 0xFF, bg_color & 0xFF,
                 text);
    } else if (fg_truecolor) {
        snprintf(buf, sizeof(buf),
                 "\x1b[38;2;%u;%u;%um\x1b[%sm%s\x1b[0m",
                 (fg_color >> 16) & 0xFF, (fg_color >> 8) & 0xFF, fg_color & 0xFF,
                 ansi_bg_codes[(bg_color > 15 ? 0 : bg_color)],
                 text);
    } else if (bg_truecolor) {
        snprintf(buf, sizeof(buf),
                 "\x1b[%sm\x1b[48;2;%u;%u;%um%s\x1b[0m",
                 ansi_fg_codes[(fg_color > 15 ? 7 : fg_color)],
                 (bg_color >> 16) & 0xFF, (bg_color >> 8) & 0xFF, bg_color & 0xFF,
                 text);
    } else {
        // 4-bit only
        const char* fg_code = ansi_fg_codes[(fg_color > 15 ? 7 : fg_color)];
        const char* bg_code = ansi_bg_codes[(bg_color > 15 ? 0 : bg_color)];
        snprintf(buf, sizeof(buf), "\x1b[%s;%sm%s\x1b[0m", fg_code, bg_code, text);
    }

    kprint(buf);
}

// Print colored text at a specific (x, y) location using ANSI codes
void kprint_color_at(int x, int y,
                     const char* text,
                     uint32_t fg_color, bool fg_truecolor,
                     uint32_t bg_color, bool bg_truecolor) {
    char buf[1024];

    // Move cursor to (y,x) and save it (y = row, x = column)
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", y, x);  // ANSI escape to move cursor
    kprint(buf);

    // Print the colored text at the new cursor position
    kprint_color(text, fg_color, fg_truecolor, bg_color, bg_truecolor);
}

void putPixel(int x, int y, uint32_t color) {
    if (!fb) {
        fb = framebuffer_request.response->framebuffers[0];
        if (!fb) return;
    }

    if (x < 0 || y < 0 || x >= (int)fb->width || y >= (int)fb->height)
        return;

    uint8_t* pixel = (uint8_t*)fb->address + y * fb->pitch + x * 4;

    // BGRA format (common)
    pixel[0] = color & 0xFF;          // Blue
    pixel[1] = (color >> 8) & 0xFF;   // Green
    pixel[2] = (color >> 16) & 0xFF;  // Red
    pixel[3] = 0xFF;                  // Alpha (opaque)
}


void draw_rect(int x, int y, int w, int h, uint32_t color) {
    for (int py = y; py < y + h; py++) {
        for (int px = x; px < x + w; px++) {
            putPixel(px, py, color);
        }
    }
}

