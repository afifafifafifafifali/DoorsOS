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

void kprint_color(const char* text, uint8_t fg_color_code, uint8_t bg_color_code);
// Create a new window
HWND CreateWindow(const char* title, int x, int y, int width, int height, COLORREF bgColor);

// Render a string in the window
void DrawText(HWND hwnd, int x, int y, const char* text, COLORREF color);

// Redraw the window (calls your framebuffer/text draw logic)
void RedrawWindow(HWND hwnd);

// Destroys a window
void DestroyWindow(HWND hwnd);

// Optional: get client rect
void GetClientRect(HWND hwnd, RECT* outRect);

#ifdef __cplusplus
}
#endif

#endif // DOORS_WINDOWS_H
