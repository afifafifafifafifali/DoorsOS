#ifndef __EZLIBC_PS2_KBIO_H
#define __EZLIBC_PS2_KBIO_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FD_KBIO_EVENTS    3

#define KBIO_KEY_NONE     0
#define KBIO_KEY_ESC      0x01
#define KBIO_KEY_ENTER    0x1B
#define KBIO_KEY_BACKSPACE 0x0E
#define KBIO_KEY_TAB      0x0F

#define KBIO_KEY_UP       0x80
#define KBIO_KEY_DOWN     0x81
#define KBIO_KEY_RIGHT    0x82
#define KBIO_KEY_LEFT     0x83

int ps2_kbio_init(void);
int ps2_kbio_close(void);

int ps2_kbio_getchar_nb(void);
int ps2_kbio_getchar(void);

int ps2_kbio_kbhit(void);

#ifdef __cplusplus
}
#endif

#endif