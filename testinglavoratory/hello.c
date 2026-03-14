#include <stdint.h>

#define SYS_WRITE 1
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

static inline uint64_t sys_print(const char* buf, uint64_t count) {
    return syscall(SYS_WRITE, 1, (uint64_t)buf, count);
}

// Prints a null-terminated string
static void print_str(const char *s) {
    const char *p = s;
    while(*p) p++;
    sys_print(s, p-s);
}

void main_program(int argc, char **argv) {
    for(int i = 0; i < argc; i++) {
        print_str("argv[");
        char c = '0' + i;
        sys_print(&c, 1);
        print_str("] = ");
        print_str(argv[i]);
        print_str("\n");
    }
}

void _start(int argc, char **argv,char **envp) {
    const char msg[] = "Hello, DoorsOS! HI FROM C FILE!\n";
    sys_print(msg, sizeof(msg)-1);
    main_program(argc, argv);

     print_str("Environment variables:\n");
    if (!envp[0]) {
        print_str("  <none>\n");
    } else {
        for (int i = 0; envp[i]; i++) {
            print_str("envp[");
            char c = '0' + i;
            sys_print(&c, 1);
            print_str("] = ");
            print_str(envp[i]);
            print_str("\n");
        }
}
}
//gcc -nostdlib -nodefaultlibs -fno-stack-protector --save-temp -Wl,-e,_start -o test_add hello.c