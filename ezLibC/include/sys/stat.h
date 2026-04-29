#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <stdint.h>
#include <sys/types.h>
/* File type flags (minimal) */
#define S_IFMT  0xF000
#define S_IFREG 0x8000
#define S_IFDIR 0x4000

#include <stdio.h>

/* Stub stat function */
static inline int stat(const char *path, struct stat *buf) {
    (void)path;

    if (!buf) return -1;

    buf->st_mode = S_IFREG;
    buf->st_size = 0;

    return 0;
}
static inline int mkdir(const char *path, int mode){
    return 0;
}

#endif