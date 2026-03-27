#include <stdint.h>

extern int global_val;
extern void fptr_target();

static inline uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2, 
                                uint64_t arg3, uint64_t arg4, uint64_t arg5, 
                                uint64_t arg6) {
    uint64_t ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), 
          "r"((uint64_t)arg4), "r"((uint64_t)arg5), "r"((uint64_t)arg6)
        : "rcx", "r11", "memory", "cc"
    );
    return ret;
}

// Overloaded wrappers for convenience (3-arg version)
static inline uint64_t syscall3(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    return syscall(num, arg1, arg2, arg3, 0, 0, 0);
}

// Convenience wrappers - Linux convention: write(fd, buf, count)
static inline uint64_t sys_print_write(int fd, const char* buf, uint64_t count) {
    return syscall3(67671, (uint64_t)fd, (uint64_t)buf, count);
}

void print_global() {
    char buf[2];
    buf[0] = '0' + global_val; // relocation required
    buf[1] = '\n';
    sys_print(buf, 2);
}

void _start() {
    const char msg[] = "Testing relocations!\n";
    sys_print(msg, sizeof(msg)-1);

    print_global();

    void (*fp)() = fptr_target; // relocation required
    fp();

}