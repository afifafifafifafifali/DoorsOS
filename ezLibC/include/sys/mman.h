/* sys/mman.h for ezLibC */

#ifndef __EZLIBC_SYS_MMAN_H
#define __EZLIBC_SYS_MMAN_H 1

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* mmap flags */
#define MAP_SHARED     0x01
#define MAP_PRIVATE    0x02
#define MAP_ANONYMOUS  0x20
#define MAP_ANON       MAP_ANONYMOUS
#define MAP_FIXED      0x10
#define MAP_NORESERVE  0x4000

/* mmap protection flags */
#define PROT_NONE     0x00
#define PROT_READ     0x01
#define PROT_WRITE    0x02
#define PROT_EXEC     0x04

/* msync flags removed — deprecated */

/* mmap failed return */
#define MAP_FAILED    ((void*)-1)

/* Functions — implemented in src/mman.c */

void* mmap(void* addr, size_t length, int prot, int flags, int fd, size_t offset);
int munmap(void* addr, size_t length);

#ifdef __cplusplus
}
#endif

#endif
