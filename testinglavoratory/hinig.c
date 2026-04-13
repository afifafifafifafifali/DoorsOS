#include <stdint.h>
#include <stddef.h>

// ============================================================
// Syscall numbers (matching your kernel)
// ============================================================
#define SYS_PRINT_WRITE   67671
#define SYS_FUCK_YOU      67673
#define SYS_EXIT          60
#define SYS_GETPID        39

// ============================================================
// Syscall wrapper
// ============================================================
static inline uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2,
                                uint64_t arg3, uint64_t arg4, uint64_t arg5,
                                uint64_t arg6) {
    uint64_t ret;
    register uint64_t r10 asm("r10") = arg4;
    register uint64_t r8  asm("r8")  = arg5;
    register uint64_t r9  asm("r9")  = arg6;

    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory", "cc"
    );
    return ret;
}

static inline uint64_t syscall3(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    return syscall(num, arg1, arg2, arg3, 0, 0, 0);
}

static inline uint64_t sys_print_write(int fd, const char* buf, uint64_t count) {
    return syscall3(SYS_PRINT_WRITE, (uint64_t)fd, (uint64_t)buf, count);
}

static inline uint64_t sys_fuck_you(void) {
    return syscall(SYS_FUCK_YOU, 11, 22, 33, 44, 55, 66);
}

// ============================================================
// Auxiliary vector types
// ============================================================
#define AT_NULL   0
#define AT_IGNORE 1
#define AT_EXECFD 2
#define AT_PHDR   3
#define AT_PHENT  4
#define AT_PHNUM  5
#define AT_PAGESZ 6
#define AT_BASE   7
#define AT_FLAGS  8
#define AT_ENTRY  9
#define AT_NOTELF 10
#define AT_UID    11
#define AT_EUID   12
#define AT_GID    13
#define AT_EGID   14
#define AT_CLKTCK 17
#define AT_RANDOM 25
#define AT_EXECFN 31

typedef struct {
    uint64_t a_type;
    uint64_t a_un;
} auxv_t;

// ============================================================
// Helper functions
// ============================================================
static void print_str(const char *s) {
    const char *p = s;
    while(*p) p++;
    sys_print_write(1, s, p - s);
}

static void print_hex(uint64_t v) {
    char buf[19] = "0x";
    static const char hex[] = "0123456789abcdef";
    for (int i = 0; i < 16; i++) {
        buf[2 + i] = hex[(v >> (60 - i * 4)) & 0xf];
    }
    buf[18] = '\0';
    sys_print_write(1, buf, 18);
}

static void print_int(int num) {
    if (num == 0) { sys_print_write(1, "0", 1); return; }
    char buf[20]; int i = 0, neg = 0;
    if (num < 0) { neg = 1; num = -num; }
    while (num > 0) { buf[i++] = '0' + (num % 10); num /= 10; }
    if (neg) buf[i++] = '-';
    for (int j = 0; j < i / 2; j++) {
        char t = buf[j]; buf[j] = buf[i-j-1]; buf[i-j-1] = t;
    }
    sys_print_write(1, buf, i);
}

static void print_ulong(uint64_t v) {
    char buf[22];
    int i = sizeof(buf) - 1;
    buf[i] = '\0';
    if (v == 0) {
        buf[--i] = '0';
    } else {
        while (v > 0) {
            buf[--i] = (v % 10) + '0';
            v /= 10;
        }
    }
    sys_print_write(1, &buf[i], sizeof(buf) - 1 - i);
}

static const char* auxv_type_to_str(uint64_t type) {
    switch (type) {
        case AT_NULL:   return "AT_NULL";
        case AT_IGNORE: return "AT_IGNORE";
        case AT_EXECFD: return "AT_EXECFD";
        case AT_PHDR:   return "AT_PHDR";
        case AT_PHENT:  return "AT_PHENT";
        case AT_PHNUM:  return "AT_PHNUM";
        case AT_PAGESZ: return "AT_PAGESZ";
        case AT_BASE:   return "AT_BASE";
        case AT_FLAGS:  return "AT_FLAGS";
        case AT_ENTRY:  return "AT_ENTRY";
        case AT_NOTELF: return "AT_NOTELF";
        case AT_UID:    return "AT_UID";
        case AT_EUID:   return "AT_EUID";
        case AT_GID:    return "AT_GID";
        case AT_EGID:   return "AT_EGID";
        case AT_CLKTCK: return "AT_CLKTCK";
        case AT_RANDOM: return "AT_RANDOM";
        case AT_EXECFN: return "AT_EXECFN";
        default:        return "AT_UNKNOWN";
    }
}



void _start(void) {
    print_str("/n hi nigga");
}

// gcc -nostdlib -nodefaultlibs -fno-stack-protector -static -pie -Wl,-e,_start -o test_add hinig.c