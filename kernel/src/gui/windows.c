#include "windows.h"
#include "../gfx/printf.h"
#include "../gfx/term.h"
#include "../mem/heap.h"
#include "../bootloader.h"
#include "../flanterm/src/flanterm_backends/fb.h"
#include <stdbool.h>

extern volatile struct limine_framebuffer_request framebuffer_request;
static struct limine_framebuffer* fb = NULL;



// Map simple 0–15 color indexes to ANSI codes (standard + bright)
static const char* ansi_fg_codes[] = {
    "30", "31", "32", "33", "34", "35", "36", "37",     // 0-7  normal
    "90", "91", "92", "93", "94", "95", "96", "97"      // 8-15 bright
};

static const char* ansi_bg_codes[] = {
    "40", "41", "42", "43", "44", "45", "46", "47",     // 0-7  normal
    "100", "101", "102", "103", "104", "105", "106", "107" // 8–15 bright
};

// Print text with ANSI foreground and background color codes
void kprint_color(const char* text, uint8_t fg_color_code, uint8_t bg_color_code) {
    char buf[512];

    // Clamp to 0–15 range
    if (fg_color_code > 15) fg_color_code = 7;
    if (bg_color_code > 15) bg_color_code = 0;

    const char* fg_code = ansi_fg_codes[fg_color_code];
    const char* bg_code = ansi_bg_codes[bg_color_code];

    // ESC[fg;bgmTEXTESC[0m
    snprintf(buf, sizeof(buf), "\x1b[%s;%sm%s\x1b[0m", fg_code, bg_code, text);
    kprint(buf);
}

// ANSI escape helper to move cursor to (col, row), 1-based
static void kprint_move_cursor(int col, int row) {
    // Construct ANSI escape sequence like "\e[<row>;<col>H"
    char buf[16];
    snprintf(buf, sizeof(buf), "\e[%d;%dH", row, col);
    kprint(buf);
}

WINDOW* __windows[MAX_WINDOWS] = { 0 };
static uint32_t window_id_counter = 1;

// CreateWindow just stores window data; no pixel bg fill since using kprint
HWND CreateWindow(const char* title, int x, int y, int width, int height, COLORREF bgColor) {
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (__windows[i] == NULL) {
            WINDOW* win = (WINDOW*)malloc(sizeof(WINDOW));
            if (!win) return NULL;
            win->x = x;
            win->y = y;
            win->width = width;
            win->height = height;
            win->bgColor = bgColor; // ignored for now
            strncpy(win->title, title, 63);
            win->title[63] = 0;
            win->id = window_id_counter++;
            win->active = 1;
            __windows[i] = win;

            // Clear window area by printing spaces
            for (int row = 0; row < height / 8; row++) {
                kprint_move_cursor((x / 8) + 1, (y / 8) + 1 + row);
                for (int col = 0; col < width / 8; col++) {
                    kprint(" ");
                }
            }

            // Print window title at top-left corner of window
            kprint_move_cursor((x / 8) + 1, (y / 8) + 1);
            kprint(title);

            return (HWND)win;
        }
    }
    return NULL;
}

// DrawText prints text at window-relative (x,y)
void DrawText(HWND hwnd, int x, int y, const char* text, COLORREF color) {
    (void)color; // ignore color for now
    WINDOW* win = (WINDOW*)hwnd;
    if (!win || !text) return;

    // Calculate absolute cursor position (1-based)
    int cursor_col = (win->x + x) / 8 + 1;
    int cursor_row = (win->y + y) / 8 + 1;

    kprint_move_cursor(cursor_col, cursor_row);
    kprint(text);
}

void DestroyWindow(HWND hwnd) {
    WINDOW* win = (WINDOW*)hwnd;
    if (!win) return;
    for (int i = 0; i < MAX_WINDOWS; i++) {
        if (__windows[i] == win) {
            free(win);
            __windows[i] = NULL;
            break;
        }
    }
}

void GetClientRect(HWND hwnd, RECT* outRect) {
    WINDOW* win = (WINDOW*)hwnd;
    if (!win || !outRect) return;
    outRect->left = 0;
    outRect->top = 0;
    outRect->right = win->width;
    outRect->bottom = win->height;
}

void RedrawWindow(HWND hwnd) {
    WINDOW* win = (WINDOW*)hwnd;
    if (!win) return;

    // Clear the window area
    for (int row = 0; row < win->height / 8; row++) {
        kprint_move_cursor((win->x / 8) + 1, (win->y / 8) + 1 + row);
        for (int col = 0; col < win->width / 8; col++) {
            kprint(" ");
        }
    }

    // Print title again
    kprint_move_cursor((win->x / 8) + 1, (win->y / 8) + 1);
    kprint(win->title);
}
