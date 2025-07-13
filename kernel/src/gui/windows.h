#ifndef DOORS_WINDOWS_H
#define DOORS_WINDOWS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif

// Basic type definitions
typedef void* HWND;
typedef uint32_t COLORREF;
typedef struct tagRECT {
    int left;
    int top;
    int right;
    int bottom;
} RECT;

// Simple window structure
typedef struct tagWINDOW {
    int x, y;              // Top-left position
    int width, height;     // Size
    COLORREF bgColor;      // Background color
    char title[64];        // Window title
    // Additional internal state:
    uint32_t id;
    uint8_t active;
} WINDOW;

// Global window count
#define MAX_WINDOWS 32
extern WINDOW* __windows[MAX_WINDOWS];
// True color
void kprint_color(const char* text,
                  uint32_t fg_color, bool fg_truecolor,
                  uint32_t bg_color, bool bg_truecolor);
// Print colored text at a specific (x, y) location using ANSI codes
void kprint_color_at(int x, int y,
                     const char* text,
                     uint32_t fg_color, bool fg_truecolor,
                     uint32_t bg_color, bool bg_truecolor);      
                     
                     
static inline void draw_char(int x, int y, char c, uint32_t fg, uint32_t bg) {
    char str[2] = { c, '\0' };
    kprint_color_at(x, y, str, fg, true, bg, true);
}
void putPixel(int x, int y, uint32_t color);        
void draw_rect(int x, int y, int w, int h, uint32_t color);     
void draw_circle(int cx, int cy, int radius, uint32_t color);        
#endif // DOORS_WINDOWS_H
