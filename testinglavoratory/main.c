#include <stdint.h>

extern int global_val;
extern void fptr_target();

static inline uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    uint64_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(num),"D"(arg1),"S"(arg2),"d"(arg3) : "rcx","r11","r8","r9","r10","memory","cc");
    return ret;
}
#define SYS_WRITE 67671
static inline void sys_print(const char* buf, uint64_t count) { syscall(SYS_WRITE, 1, (uint64_t)buf, count); }

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