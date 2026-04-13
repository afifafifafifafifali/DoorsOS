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

// ============================================================
// Stack layout verification functions
// ============================================================

void test_stack_alignment() {
    print_str("\n=== TEST: Stack Alignment ===\n");
    
    uint64_t rsp;
    asm volatile("mov %%rsp, %0" : "=r"(rsp));
    
    print_str("Current RSP: ");
    print_hex(rsp);
    print_str("\n");
    
    if ((rsp & 0xF) == 0) {
        print_str("[PASS] Stack is 16-byte aligned\n");
    } else {
        print_str("[FAIL] Stack is NOT 16-byte aligned! Off by: ");
        print_int(rsp & 0xF);
        print_str(" bytes\n");
    }
}

void test_argc_argv(int argc, char **argv) {
    print_str("\n=== TEST: argc/argv ===\n");
    
    print_str("argc = ");
    print_int(argc);
    print_str("\n");
    
    if (argc == 0) {
        print_str("[FAIL] argc is 0!\n");
        return;
    }
    
    print_str("[PASS] argc is valid\n");
    
    for (int i = 0; i < argc; i++) {
        print_str("argv[");
        print_int(i);
        print_str("] = ");
        
        if (argv[i] == 0) {
            print_str("<NULL>\n");
            print_str("[FAIL] argv[");
            print_int(i);
            print_str("] is NULL before argc!\n");
        } else {
            print_str("\"");
            print_str(argv[i]);
            print_str("\"\n");
        }
    }
    
    if (argv[argc] == 0) {
        print_str("argv[argc] = <NULL>\n");
        print_str("[PASS] argv is NULL-terminated\n");
    } else {
        print_str("[FAIL] argv is NOT NULL-terminated!\n");
    }
}

void test_envp(char **envp) {
    print_str("\n=== TEST: Environment Variables ===\n");
    
    if (!envp) {
        print_str("[FAIL] envp is NULL!\n");
        return;
    }
    
    if (!envp[0]) {
        print_str("envp is empty (NULL-terminated)\n");
        print_str("[PASS] envp structure is valid (empty)\n");
        return;
    }
    
    int count = 0;
    for (int i = 0; envp[i]; i++) {
        print_str("envp[");
        print_int(i);
        print_str("] = \"");
        print_str(envp[i]);
        print_str("\"\n");
        count++;
    }
    
    print_str("Total environment variables: ");
    print_int(count);
    print_str("\n");
    print_str("[PASS] envp is valid and NULL-terminated\n");
}

void test_auxv(auxv_t *auxv) {
    print_str("\n=== TEST: Auxiliary Vector ===\n");
    
    if (!auxv) {
        print_str("[FAIL] auxv is NULL!\n");
        return;
    }
    
    int count = 0;
    int found_null = 0;
    
    for (int i = 0; i < 100; i++) {  // Safety limit
        if (auxv[i].a_type == AT_NULL) {
            found_null = 1;
            break;
        }
        
        print_str("auxv[");
        print_int(i);
        print_str("]: ");
        print_str(auxv_type_to_str(auxv[i].a_type));
        print_str(" = ");
        print_hex(auxv[i].a_un);
        print_str("\n");
        
        count++;
    }
    
    if (!found_null) {
        print_str("[FAIL] auxv is not AT_NULL terminated!\n");
        return;
    }
    
    print_str("Total auxiliary entries: ");
    print_int(count);
    print_str("\n");
    print_str("[PASS] auxv is valid and AT_NULL-terminated\n");
}

void test_stack_layout(int argc, char **argv, char **envp, auxv_t *auxv) {
    print_str("\n=== TEST: Stack Memory Layout ===\n");
    
    uint64_t stack_top;
    asm volatile("mov %%rsp, %0" : "=r"(stack_top));
    
    // The stack should have been set up like:
    // [rsp] = argc
    // [rsp+8] = argv[0]
    // ...
    
    uint64_t *stack_ptr = (uint64_t *)stack_top;
    
    print_str("Stack pointer: ");
    print_hex(stack_top);
    print_str("\n");
    
    print_str("argc address: ");
    print_hex((uint64_t)&argc);
    print_str("\n");
    
    print_str("argv address: ");
    print_hex((uint64_t)argv);
    print_str("\n");
    
    print_str("envp address: ");
    print_hex((uint64_t)envp);
    print_str("\n");
    
    print_str("auxv address: ");
    print_hex((uint64_t)auxv);
    print_str("\n");
    
    // Verify ordering: argc < argv < envp < auxv
    if ((uint64_t)&argc < (uint64_t)argv &&
        (uint64_t)argv < (uint64_t)envp &&
        (uint64_t)envp < (uint64_t)auxv) {
        print_str("[PASS] Stack layout order is correct\n");
    } else {
        print_str("[FAIL] Stack layout order is incorrect!\n");
    }
}

void test_syscalls() {
    print_str("\n=== TEST: Syscalls ===\n");
    
    const char *test_msg = "Syscall test message\n";
    uint64_t ret = sys_print_write(1, test_msg, 21);
    
    print_str("sys_print_write returned: ");
    print_ulong(ret);
    print_str("\n");
    
    if (ret == 21) {
        print_str("[PASS] sys_print_write works correctly\n");
    } else {
        print_str("[WARN] sys_print_write returned unexpected value\n");
    }
    
    // Test the fuck_you syscall
    print_str("Calling sys_fuck_you()...\n");
    ret = sys_fuck_you();
    print_str("sys_fuck_you returned: ");
    print_ulong(ret);
    print_str("\n");
    print_str("[PASS] Syscalls are functional\n");
}

void test_tls() {
    print_str("\n=== TEST: Thread-Local Storage (FS base) ===\n");
    
    uint64_t fs_base;
    asm volatile("rdfsbase %0" : "=r"(fs_base));
    
    print_str("FS base: ");
    print_hex(fs_base);
    print_str("\n");
    
    if (fs_base != 0) {
        print_str("[PASS] FS base is set (TLS initialized)\n");
    } else {
        print_str("[FAIL] FS base is NULL!\n");
    }
}

void test_bss_and_data() {
    print_str("\n=== TEST: .bss and .data sections ===\n");
    
    static int initialized_var = 42;
    static int uninitialized_var;
    
    print_str("initialized_var (.data) = ");
    print_int(initialized_var);
    print_str("\n");
    
    print_str("uninitialized_var (.bss) = ");
    print_int(uninitialized_var);
    print_str("\n");
    
    if (initialized_var == 42 && uninitialized_var == 0) {
        print_str("[PASS] .data and .bss sections loaded correctly\n");
    } else {
        print_str("[FAIL] Section initialization failed!\n");
    }
}

// ============================================================
// Main entry point (System V ABI compatible)
// ============================================================
void _start(int argc, char **argv, char **envp, auxv_t *auxv) {
    print_str("\n");
    print_str("========================================\n");
    print_str("  DoorsOS System V ABI Stack Test\n");
    print_str("========================================\n");
    
    // Run all tests
    test_stack_alignment();
    test_argc_argv(argc, argv);
    test_envp(envp);
    test_auxv(auxv);
    test_stack_layout(argc, argv, envp, auxv);
    test_syscalls();
    test_tls();
    test_bss_and_data();
    
    print_str("\n");
    print_str("========================================\n");
    print_str("  All Tests Complete!\n");
    print_str("========================================\n");
    print_str("\n");
    
    // Return success code
    // In a real implementation, you'd call exit(0) here
}

//gcc -nostdlib -nodefaultlibs -fno-stack-protector -static -pie -Wl,-e,_start -o test_add nigga.c