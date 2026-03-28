#include <stdint.h>
#include <stddef.h>
// ============================================================
// Syscall numbers
// ============================================================
#define SYS_PRINT_WRITE   67671
#define SYS_FUCK_YOU      67673
#define SYS_UNAME         63
#define SYS_LSEEK         49
#define SYS_MMAP          214
#define SYS_MUNMAP        215

// mmap flags
#define PROT_NONE         0x0
#define PROT_READ         0x1
#define PROT_WRITE        0x2
#define PROT_EXEC         0x4
#define MAP_SHARED        0x01
#define MAP_PRIVATE       0x02
#define MAP_ANONYMOUS     0x20
#define MAP_FIXED         0x10

// lseek whence
#define SEEK_SET          0
#define SEEK_CUR          1
#define SEEK_END          2

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

static inline uint64_t syscall0(uint64_t num) {
    return syscall(num, 0, 0, 0, 0, 0, 0);
}

static inline uint64_t syscall1(uint64_t num, uint64_t arg1) {
    return syscall(num, arg1, 0, 0, 0, 0, 0);
}

static inline uint64_t syscall2(uint64_t num, uint64_t arg1, uint64_t arg2) {
    return syscall(num, arg1, arg2, 0, 0, 0, 0);
}

static inline uint64_t syscall3(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    return syscall(num, arg1, arg2, arg3, 0, 0, 0);
}

static inline uint64_t syscall6(uint64_t num, uint64_t arg1, uint64_t arg2,
                                uint64_t arg3, uint64_t arg4, uint64_t arg5,
                                uint64_t arg6) {
    return syscall(num, arg1, arg2, arg3, arg4, arg5, arg6);
}

// ============================================================
// Syscall wrappers
// ============================================================
static inline uint64_t sys_print_write(int fd, const char* buf, uint64_t count) {
    return syscall3(SYS_PRINT_WRITE, (uint64_t)fd, (uint64_t)buf, count);
}

static inline uint64_t sys_fuck_you(void) {
    return syscall(SYS_FUCK_YOU, 11, 22, 33, 44, 55, 66);
}

struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
};

static inline int64_t sys_uname(struct utsname* buf) {
    return syscall1(SYS_UNAME, (uint64_t)buf);
}

static inline int64_t sys_lseek(int fd, int64_t offset, int whence) {
    return syscall3(SYS_LSEEK, (uint64_t)fd, (uint64_t)offset, (uint64_t)whence);
}

static inline void* sys_mmap(void* addr, uint64_t length, int prot, int flags,
                             int fd, int64_t offset) {
    return (void*)syscall6(SYS_MMAP, (uint64_t)addr, length, (uint64_t)prot,
                           (uint64_t)flags, (uint64_t)fd, (uint64_t)offset);
}

static inline int64_t sys_munmap(void* addr, uint64_t length) {
    return syscall2(SYS_MUNMAP, (uint64_t)addr, length);
}

// ============================================================
// Helper functions
// ============================================================
static void print_str(const char *s) {
    const char *p = s;
    while(*p) p++;
    sys_print_write(1, s, p - s);
}

static void print_int(int num) {
    if (num == 0) {
        sys_print_write(1, "0", 1);
        return;
    }

    char buf[20];
    int i = 0;
    int is_negative = 0;

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buf[i++] = '-';
    }

    for (int j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = tmp;
    }

    sys_print_write(1, buf, i);
}

static void print_hex(uint64_t num) {
    if (num == 0) {
        sys_print_write(1, "0x0", 3);
        return;
    }

    char buf[20];
    int i = 0;

    while (num > 0) {
        int digit = num % 16;
        buf[i++] = (digit < 10) ? ('0' + digit) : ('a' + digit - 10);
        num /= 16;
    }

    sys_print_write(1, "0x", 2);
    for (int j = i - 1; j >= 0; j--) {
        char c[1] = {buf[j]};
        sys_print_write(1, c, 1);
    }
}

static void print_long(int64_t num) {
    if (num == 0) {
        sys_print_write(1, "0", 1);
        return;
    }

    char buf[25];
    int i = 0;
    int is_negative = 0;

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buf[i++] = '-';
    }

    for (int j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = tmp;
    }

    sys_print_write(1, buf, i);
}

// ============================================================
// Test functions
// ============================================================

void test_uname() {
    print_str("\n=== TEST: sys_uname ===\n");
    
    struct utsname u;
    int64_t ret = sys_uname(&u);
    
    if (ret == 0) {
        print_str("  sysname:   ");
        print_str(u.sysname);
        print_str("\n");

        print_str("  nodename:  ");
        print_str(u.nodename);
        print_str("\n");

        print_str("  release:   ");
        print_str(u.release);
        print_str("\n");

        print_str("  version:   ");
        print_str(u.version);
        print_str("\n");

        print_str("  machine:   ");
        print_str(u.machine);
        print_str("\n");
        
        print_str("  [PASS] sys_uname succeeded\n");
    } else {
        print_str("  [FAIL] sys_uname failed with return: ");
        print_long(ret);
        print_str("\n");
    }
}

void test_lseek() {
    print_str("\n=== TEST: sys_lseek ===\n");
    
    int64_t ret;
    
    ret = sys_lseek(-1, 100, SEEK_SET);
    print_str("  lseek(-1, 100, SEEK_SET) = ");
    print_long(ret);
    print_str(" (expected: negative/error)\n");
    
    ret = sys_lseek(0, 0, SEEK_CUR);
    print_str("  lseek(0, 0, SEEK_CUR) = ");
    print_long(ret);
    print_str("\n");
    
    ret = sys_lseek(1, 0, SEEK_CUR);
    print_str("  lseek(1, 0, SEEK_CUR) = ");
    print_long(ret);
    print_str("\n");
    
    print_str("  [INFO] lseek tested\n");
}

void test_mmap_anonymous() {
    print_str("\n=== TEST: sys_mmap (anonymous) ===\n");
    
    print_str("  Test 1: Basic anonymous mapping (4KB, RW)\n");
    void* addr1 = sys_mmap(NULL, 4096, PROT_READ | PROT_WRITE, 
                           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    
    if (addr1 == (void*)-1 || addr1 == NULL) {
        print_str("    [FAIL] mmap returned NULL/-1\n");
    } else {
        print_str("    [PASS] mmap returned: ");
        print_hex((uint64_t)addr1);
        print_str("\n");
        
        volatile uint64_t* ptr = (volatile uint64_t*)addr1;
        ptr[0] = 0xDEADBEEFCAFEBABEULL;
        ptr[1] = 0x1234567890ABCDEFULL;
        
        print_str("    Read back: ");
        print_hex(ptr[0]);
        print_str(", ");
        print_hex(ptr[1]);
        print_str("\n");
        
        if (ptr[0] == 0xDEADBEEFCAFEBABEULL && ptr[1] == 0x1234567890ABCDEFULL) {
            print_str("    [PASS] Memory verification OK\n");
        } else {
            print_str("    [FAIL] Memory verification FAILED\n");
        }
        
        int64_t unmap_ret = sys_munmap(addr1, 4096);
        print_str("    munmap returned: ");
        print_long(unmap_ret);
        print_str("\n");
    }
    
    print_str("\n  Test 2: Larger mapping (8KB)\n");
    void* addr2 = sys_mmap(NULL, 8192, PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    
    if (addr2 == (void*)-1 || addr2 == NULL) {
        print_str("    [FAIL] mmap returned NULL/-1\n");
    } else {
        print_str("    [PASS] mmap returned: ");
        print_hex((uint64_t)addr2);
        print_str("\n");
        
        volatile uint8_t* bytes = (volatile uint8_t*)addr2;
        for (int i = 0; i < 8192; i++) {
            bytes[i] = (uint8_t)(i & 0xFF);
        }
        
        int pass = 1;
        for (int i = 0; i < 8192; i++) {
            if (bytes[i] != (uint8_t)(i & 0xFF)) {
                pass = 0;
                break;
            }
        }
        
        if (pass) {
            print_str("    [PASS] 8KB pattern OK\n");
        } else {
            print_str("    [FAIL] Pattern FAILED\n");
        }
        
        sys_munmap(addr2, 8192);
    }
    
    print_str("\n  Test 3: Multiple mappings\n");
    void* m1 = sys_mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    void* m2 = sys_mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    void* m3 = sys_mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    
    print_str("    m1="); print_hex((uint64_t)m1); print_str("\n");
    print_str("    m2="); print_hex((uint64_t)m2); print_str("\n");
    print_str("    m3="); print_hex((uint64_t)m3); print_str("\n");
    
    if (m1 && m1 != (void*)-1) ((volatile uint64_t*)m1)[0] = 0xAAAA;
    if (m2 && m2 != (void*)-1) ((volatile uint64_t*)m2)[0] = 0xBBBB;
    if (m3 && m3 != (void*)-1) ((volatile uint64_t*)m3)[0] = 0xCCCC;
    
    print_str("    Values: ");
    if (m1 && m1 != (void*)-1) print_hex(((volatile uint64_t*)m1)[0]);
    print_str(", ");
    if (m2 && m2 != (void*)-1) print_hex(((volatile uint64_t*)m2)[0]);
    print_str(", ");
    if (m3 && m3 != (void*)-1) print_hex(((volatile uint64_t*)m3)[0]);
    print_str("\n");
    
    if (m1 && m1 != (void*)-1) sys_munmap(m1, 4096);
    if (m2 && m2 != (void*)-1) sys_munmap(m2, 4096);
    if (m3 && m3 != (void*)-1) sys_munmap(m3, 4096);
}

// ============================================================
// Main program
// ============================================================
void main_program(int argc, char **argv) {
    print_str("\n=== Unix Syscall Tests ===\n");
    
    for(int i = 0; i < argc; i++) {
        print_str("argv[");
        print_int(i);
        print_str("] = ");
        print_str(argv[i]);
        print_str("\n");
    }
    
    test_uname();
    test_lseek();
    test_mmap_anonymous();
    
    print_str("\n=== All Tests Complete ===\n");
}

void _start(int argc, char **argv, char **envp) {
    const char msg[] = "Hello, DoorsOS! Unix Syscall Test Edition!\n";
    sys_print_write(1, msg, sizeof(msg) - 1);
    
    main_program(argc, argv);
    
    print_str("\nEnvironment:\n");
    if (!envp || !envp[0]) {
        print_str("  <none>\n");
    } else {
        for (int i = 0; envp[i]; i++) {
            print_str("  envp[");
            print_int(i);
            print_str("] = ");
            print_str(envp[i]);
            print_str("\n");
        }
    }
    
    sys_fuck_you();
    print_str("\nDone.\n");
}

// Compile with:
// gcc -nostdlib -nodefaultlibs -fno-stack-protector --save-temps -fPIC -fPIE -Wl,-e,_start -o test_add hello.c
