#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// Syscall numbers (Linux-compatible where possible)
#define SYS_READ    0
#define SYS_WRITE   1
#define SYS_OPEN    2
#define SYS_CLOSE   3
#define SYS_EXIT    60
#define SYS_YIELD   100
#define SYS_FORK    101
#define SYS_EXEC    102
#define SYS_WAIT    103
#define SYS_GETPID  104

// Syscall wrapper macro using int 0x80
// Arguments: rax=syscall_num, rdi=arg1, rsi=arg2, rdx=arg3
// Returns: result in rax
static inline uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    uint64_t ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3)
        : "rcx", "r11", "r8", "r9", "r10", "memory", "cc"
    );
    return ret;
}

// Convenience wrappers
static inline uint64_t sys_write(const char* buf, uint64_t count) {
    return syscall(SYS_WRITE, (uint64_t)buf, count, 0);
}

static inline uint64_t sys_read(char* buf, uint64_t count) {
    return syscall(SYS_READ, (uint64_t)buf, count, 0);
}

static inline void sys_exit(uint64_t code) {
    syscall(SYS_EXIT, code, 0, 0);
}

static inline void sys_yield() {
    syscall(SYS_YIELD, 0, 0, 0);
}

static inline uint64_t sys_getpid() {
    return syscall(SYS_GETPID, 0, 0, 0);
}

// Initialization
void syscall_init();

// Internal handler called from assembly (C convention)
uint64_t syscall_handler_c(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3);

#endif
