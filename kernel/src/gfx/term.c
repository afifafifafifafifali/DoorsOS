#include "term.h"

#include <stddef.h>
#include "../limine.h"
#include "../libs/string.h"
#include "../flanterm/src/flanterm.h"
#include "../flanterm/src/flanterm_backends/fb.h"


static struct flanterm_context* ft_ctx;

struct flanterm_context* initialize_terminal(struct limine_framebuffer* framebuffer) {
    ft_ctx = flanterm_fb_init(
        NULL,
        NULL,
        framebuffer->address, framebuffer->width, framebuffer->height, framebuffer->pitch,
        framebuffer->red_mask_size, framebuffer->red_mask_shift,
        framebuffer->green_mask_size, framebuffer->green_mask_shift,
        framebuffer->blue_mask_size, framebuffer->blue_mask_shift,
        NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, NULL,
        NULL, 0, 0, 1,
        0, 0,
        0
    );
    return ft_ctx;
}


void kprint(const char* msg) {
    if (!ft_ctx) return;
    flanterm_write(ft_ctx, msg, strlen(msg));
}


void ftprint(struct flanterm_context* ft_ctx, const char* msg, size_t len) {
    flanterm_write(ft_ctx, msg, len);
}

void _putchar(char character) { // This func is shit, i dont wanna use it 
    char msg[2];
    msg[0] = character;
    msg[1] = '\0';
    flanterm_write(ft_ctx, msg, 2);
}
