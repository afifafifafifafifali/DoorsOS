#ifndef __EZLIBC_KBIO_H
#define __EZLIBC_KBIO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FD_KBIO_EVENTS    3

#define KBIO_KEY_NONE     0
#define KBIO_KEY_UP       1
#define KBIO_KEY_DOWN     2
#define KBIO_KEY_RIGHT    3
#define KBIO_KEY_LEFT     4

int kbhit(void);
int getch(void);
int getche(void);

#ifdef __cplusplus
}
#endif

#endif