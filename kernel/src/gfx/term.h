#pragma once
#include <stddef.h>
#include "../limine.h"
#include "../libs/string.h"

static struct flanterm_context* ft_ctx;
struct flanterm_context* initialize_terminal(struct limine_framebuffer* framebuffer);
void kprint(const char* msg);
void ftprint(struct flanterm_context* ft_ctx, const char* msg, size_t len);
void _putchar(char character);
void clear_screen(void);