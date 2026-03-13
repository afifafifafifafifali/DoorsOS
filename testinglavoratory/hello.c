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
// Linux x86-64 syscall convention:
//   rax = syscall number
//   rdi = arg1 (fd for read/write)
//   rsi = arg2 (buffer for read/write)
//   rdx = arg3 (count for read/write)
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

// Convenience wrappers - Linux convention: write(fd, buf, count)
static inline uint64_t sys_write(int fd, const char* buf, uint64_t count) {
    return syscall(SYS_WRITE, (uint64_t)fd, (uint64_t)buf, count);
}

static inline uint64_t sys_read(int fd, char* buf, uint64_t count) {
    return syscall(SYS_READ, (uint64_t)fd, (uint64_t)buf, count);
}

// Simple wrappers for common cases (write to stdout, read from stdin)
static inline uint64_t sys_print(const char* buf, uint64_t count) {
    return syscall(SYS_WRITE, 1, (uint64_t)buf, count);
}


int donald_trump(void){
    
    for(int i = 0; i < 10; i++){
        const char msg[] = "DONALD TRUMPET\n";
        sys_print(msg, sizeof(msg)-1);
    }
    return 0;

}
void _start(void) {
    const char msg[] = "Hello, DoorsOS! HI FROM C FILE!\n";
    sys_print(msg, sizeof(msg)-1);
    donald_trump();
    //return 0; 
}
//  gcc -nostdlib -nodefaultlibs -fno-stack-protector --save-temp  -Wl,-e,_start -o test_add hello.c