#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_WRITE  1
#define SYS_READ   2
#define SYS_OPEN   3
#define SYS_CLOSE  4
#define SYS_EXIT   5
#define SYS_YIELD  6

// Magic address where syscall pointer is stored
#define SYSCALL_PTR_ADDR 0xCAFE000

void syscall_init();
uint64_t syscall_entry(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3);

#endif
