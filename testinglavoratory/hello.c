// test_reloc_funcptr.c
#include <stdint.h>

static inline uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    uint64_t ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3)
        : "rcx","r11","r8","r9","r10","memory","cc"
    );
    return ret;
}

#define SYS_WRITE 67671

static inline void sys_print(const char* buf, uint64_t count) {
    syscall(SYS_WRITE, 1, (uint64_t)buf, count);
}

// GLOBAL VARIABLE OUTSIDE _start
int global_val = 7;

// A regular function
void print_global() {
    char buf[2];
    buf[0] = '0' + global_val;
    buf[1] = '\n';
    sys_print(buf, 2);
}

// Another function for function pointer test
void fptr_target() {
    const char msg[] = "Func pointer called!\n";
    sys_print(msg, sizeof(msg)-1);
}

void _start() {
    const char msg[] = "Testing relocations with function pointers!\n";
    sys_print(msg, sizeof(msg)-1);

    // Call normal function using global
    print_global();

    // FUNCTION POINTER RELOCATION
    void (*fp)() = fptr_target; // THIS MUST RELOCATE!
    fp();                       // call via pointer

    
}
//gcc -nostdlib -fPIC -fPIE -nodefaultlibs -fno-stack-protector --save-temp -Wl,-e,_start -o test_add hello.c