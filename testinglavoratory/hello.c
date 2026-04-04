#include <stdint.h>
#include <stddef.h>
// ============================================================
// Syscall numbers
// ============================================================
#define SYS_PRINT_WRITE   67671
#define SYS_FUCK_YOU      67673
#define SYS_UNAME         63
#define SYS_LSEEK         49


// lseek whence
#define SEEK_SET          0
#define SEEK_CUR          1
#define SEEK_END          2

/* mmap / munmap / mprotect / msync — standard x86_64 Linux numbers */
#define SYS_MMAP          9
#define SYS_MUNMAP        11
#define SYS_MPROTECT      10
#define SYS_MSYNC         26

/* mmap flags */
#define PROT_READ         0x1
#define PROT_WRITE        0x2
#define PROT_EXEC         0x4
#define PROT_NONE         0x0
#define MAP_SHARED        0x01
#define MAP_PRIVATE       0x02
#define MAP_ANONYMOUS     0x20
#define MAP_ANON          MAP_ANONYMOUS
#define MAP_FIXED         0x10
#define MAP_FAILED        ((void *)-1)

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

/* mmap syscall wrappers */
static inline void* sys_mmap(void* addr, uint64_t length, int prot, int flags,
                              int fd, uint64_t offset) {
    return (void*)syscall6(SYS_MMAP, (uint64_t)addr, length, prot, flags, fd, offset);
}

static inline int64_t sys_munmap(void* addr, uint64_t length) {
    return syscall2(SYS_MUNMAP, (uint64_t)addr, length);
}

static inline int64_t sys_mprotect(void* addr, uint64_t length, int prot) {
    return syscall3(SYS_MPROTECT, (uint64_t)addr, length, prot);
}

static inline int64_t sys_msync(void* addr, uint64_t length) {
    return syscall2(SYS_MSYNC, (uint64_t)addr, length);
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

#define SYS_READ              0
#define SYS_WRITE             1
#define SYS_OPEN              2
#define SYS_CLOSE             3

static inline int64_t sys_write(int fd, const void* buf, size_t count) {
    return syscall3(SYS_WRITE, (uint64_t)fd, (uint64_t)buf, count);
}

static inline int64_t sys_open(const char* pathname, int flags, int mode) {
    return syscall3(SYS_OPEN, (uint64_t)pathname, flags, mode);
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
    
    test_mmap_anonymous();
    test_mmap_mprotect();
    test_mmap_multi();
    test_uname();
    test_lseek();

    


    print_str("\n=== All Tests Complete ===\n");
}

/* ============================================================
 * mmap / mprotect / munmap / msync tests
 * ============================================================*/

void test_mmap_anonymous() {
    print_str("\n--- TEST: anonymous mmap ---\n");

    void *p = sys_mmap(0, 8192, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (p == MAP_FAILED) {
        print_str("  [FAIL] mmap returned MAP_FAILED\n");
        return;
    }
    print_str("  OK: got mapping at ");
    print_hex((uint64_t)p);
    print_str("\n");

    /* Should be zeroed */
    uint8_t *b = (uint8_t *)p;
    int ok = 1;
    for (int i = 0; i < 8192; i++) {
        if (b[i] != 0) {
            print_str("  [FAIL] byte ");
            print_int(i);
            print_str(" is ");
            print_hex(b[i]);
            print_str(", expected 0\n");
            ok = 0;
            break;
        }
    }
    if (ok) print_str("  OK: all bytes zeroed\n");

    /* Write pattern */
    for (int i = 0; i < 8192; i++)
        b[i] = (uint8_t)(i & 0xFF);

    /* Read back */
    ok = 1;
    for (int i = 0; i < 8192; i++) {
        if (b[i] != (uint8_t)(i & 0xFF)) {
            print_str("  [FAIL] pattern mismatch at ");
            print_int(i);
            print_str("\n");
            ok = 0;
            break;
        }
    }
    if (ok) print_str("  OK: pattern write/read-back passed\n");

    int64_t ret = sys_munmap(p, 8192);
    if (ret != 0) {
        print_str("  [FAIL] munmap returned ");
        print_long(ret);
        print_str("\n");
        return;
    }
    print_str("  OK: munmap succeeded\n");
    print_str("  [PASS] anonymous mmap\n");
}

void test_mmap_mprotect() {
    print_str("\n--- TEST: mprotect ---\n");

    void *p = sys_mmap(0, 4096, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) {
        print_str("  [FAIL] mmap failed\n");
        return;
    }

    ((uint8_t *)p)[0] = 0xBE;

    int64_t ret = sys_mprotect(p, 4096, PROT_READ);
    if (ret != 0) {
        print_str("  [FAIL] mprotect PROT_READ returned ");
        print_long(ret);
        print_str("\n");
        sys_munmap(p, 4096);
        return;
    }
    print_str("  OK: mprotect PROT_READ applied\n");

    if (((uint8_t *)p)[0] != 0xBE) {
        print_str("  [FAIL] data not readable after mprotect\n");
        sys_munmap(p, 4096);
        return;
    }
    print_str("  OK: data still readable\n");

    ret = sys_mprotect(p, 4096, PROT_READ | PROT_WRITE);
    if (ret != 0) {
        print_str("  [FAIL] mprotect restore returned ");
        print_long(ret);
        print_str("\n");
        sys_munmap(p, 4096);
        return;
    }
    print_str("  OK: mprotect restore applied\n");

    ((uint8_t *)p)[0] = 0xEF;
    if (((uint8_t *)p)[0] != 0xEF) {
        print_str("  [FAIL] write after mprotect restore failed\n");
        sys_munmap(p, 4096);
        return;
    }
    print_str("  OK: write after restore succeeded\n");

    sys_munmap(p, 4096);
    print_str("  [PASS] mprotect\n");
}

void test_mmap_multi() {
    print_str("\n--- TEST: multiple mappings ---\n");

    void *a = sys_mmap(0, 4096,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    void *b = sys_mmap(0, 8192,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    void *c = sys_mmap(0, 16384, PROT_READ,               MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (a == MAP_FAILED || b == MAP_FAILED || c == MAP_FAILED) {
        print_str("  [FAIL] one of the mappings failed\n");
        if (a != MAP_FAILED) sys_munmap(a, 4096);
        if (b != MAP_FAILED) sys_munmap(b, 8192);
        if (c != MAP_FAILED) sys_munmap(c, 16384);
        return;
    }

    print_str("  OK: 3 mappings created\n");

    if (a == b || b == c || a == c) {
        print_str("  [FAIL] mappings overlap\n");
    } else {
        print_str("  a = "); print_hex((uint64_t)a); print_str("\n");
        print_str("  b = "); print_hex((uint64_t)b); print_str("\n");
        print_str("  c = "); print_hex((uint64_t)c); print_str("\n");
        print_str("  OK: distinct addresses\n");
    }

    sys_munmap(a, 4096);
    sys_munmap(b, 8192);
    sys_munmap(c, 16384);
    print_str("  [PASS] multiple mappings\n");
}

void test_console_sys_write() {
    const char* msg = "HELLO VIA SYS_WRITE\n";
    sys_write(1, msg, 21);
}
void _start(int argc, char **argv, char **envp) {
  
    
    const char msg[] = "Hello, DoorsOS! Unix Syscall Test Edition!\n";
    sys_print_write(1, msg, sizeof(msg) - 1);
    
    print_str("\nEnvironment:\n");
    if (!envp || !envp[0]) {
        print_str("  <none>\n");
    } else {
        for (int i = 0; envp[i]; i++) {
            print_str("  envp[");
            print_str("INSIDE ");
            print_int(i);
            print_str("] = ");
            print_str(envp[i]);
            print_str("\n");
        }
    }
    main_program(argc, argv);
    
    
    
    test_console_sys_write();
    sys_fuck_you();
    print_str("\nDone.\n");
}

// Compile with:
// gcc -nostdlib -nodefaultlibs  --save-temps -fPIC -fPIE -Wl,-e,_start -o test_add hello.c
