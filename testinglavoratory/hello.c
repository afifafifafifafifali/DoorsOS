#include <stdint.h>
#include <stddef.h>
// ============================================================
// Syscall numbers
// ============================================================
#define SYS_PRINT_WRITE   67671
#define SYS_FUCK_YOU      67673
#define SYS_UNAME         63
#define SYS_LSEEK         49
#define SYS_IOCTL         16

/* mmap / munmap / mprotect / msync — standard x86_64 Linux numbers */
#define SYS_MMAP          9
#define SYS_MUNMAP        11
#define SYS_MPROTECT      10
#define SYS_MSYNC         26

/* Standard I/O */
#define SYS_READ          0
#define SYS_WRITE         1
#define SYS_OPEN          2
#define SYS_CLOSE         3

// lseek whence
#define SEEK_SET          0
#define SEEK_CUR          1
#define SEEK_END          2

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

/* open flags */
#define O_RDONLY          0x0001
#define O_WRONLY          0x0002
#define O_RDWR            0x0003

/* Fake VFS FD numbers (returned by open on special paths) */
#define FD_KBIO_EVENTS    3
#define FD_FRAMEBUFFER    4
#define FD_MOUSE_EVENTS   5

/* Auxiliary vector types */
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
    uint64_t a_un;  /* value or pointer */
} auxv_t;

/* ioctl commands */
#define FBIOGET_INFO      0x4601

/* ============================================================
 * Data structures (must match kernel definitions)
 * ============================================================*/

struct fb_info {
    uint64_t addr;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint16_t bpp;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
} __attribute__((packed));

struct kbio_event {
    uint8_t  scancode;
    uint8_t  pressed;
    uint8_t  shift;
    uint8_t  ctrl;
    uint8_t  alt;
};

struct mouse_state {
    uint8_t  buttons_raw;
    uint8_t  left_down   : 1;
    uint8_t  right_down  : 1;
    uint8_t  mid_down    : 1;
    uint8_t  left_pressed  : 1;
    uint8_t  left_released : 1;
    uint8_t  right_pressed : 1;
    uint8_t  right_released: 1;
    uint8_t  mid_pressed   : 1;
    uint8_t  mid_released  : 1;
    int8_t   dx;
    int8_t   dy;
    int8_t   dz;
};

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
static inline int64_t sys_open(const char* pathname, int flags, int mode) {
    return syscall3(SYS_OPEN, (uint64_t)pathname, flags, mode);
}
static inline int64_t sys_close(int fd) {
    return syscall1(SYS_CLOSE, (uint64_t)fd);
}
static inline int64_t sys_read(int fd, void* buf, uint64_t count) {
    return syscall3(SYS_READ, (uint64_t)fd, (uint64_t)buf, count);
}
static inline int64_t sys_write(int fd, const void* buf, uint64_t count) {
    return syscall3(SYS_WRITE, (uint64_t)fd, (uint64_t)buf, count);
}
static inline int64_t sys_ioctl(int fd, uint64_t req, uint64_t arg) {
    return syscall3(SYS_IOCTL, (uint64_t)fd, req, arg);
}

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

// ============================================================
// Helper functions
// ============================================================
static void print_str(const char *s) {
    const char *p = s;
    while(*p) p++;
    sys_print_write(1, s, p - s);
}

static void print_ulong(uint64_t v);
static void print_int(int num);

static void print_hex(uint64_t v) {
    char buf[19] = "0x";
    static const char hex[] = "0123456789abcdef";
    for (int i = 0; i < 16; i++) {
        buf[2 + i] = hex[(v >> (60 - i * 4)) & 0xf];
    }
    buf[18] = '\0';
    sys_print_write(1, buf, 18);
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

static void print_auxv(auxv_t *auxv) {
    print_str("\nAuxiliary Vector:\n");
    if (!auxv) {
        print_str("  <none>\n");
        return;
    }
    
    for (int i = 0; auxv[i].a_type != AT_NULL; i++) {
        print_str("  ");
        print_str(auxv_type_to_str(auxv[i].a_type));
        print_str(" (");
        print_int(auxv[i].a_type);
        print_str(") = ");
        print_ulong(auxv[i].a_un);
        print_str("\n");
    }
    print_str("  AT_NULL (end of vector)\n");
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
static void print_long(int64_t num) { print_int((int)num); }

// ============================================================
// Fake VFS tests
// ============================================================

void test_framebuffer() {
    print_str("\n--- TEST: /dev/fb0 (framebuffer) ---\n");

    int fb = sys_open("/dev/fb0", O_RDWR, 0);
    if (fb < 0) {
        print_str("  [FAIL] open /dev/fb0 returned ");
        print_long(fb);
        print_str("\n");
        return;
    }
    print_str("  OK: opened /dev/fb0 as fd=");
    print_int(fb);
    print_str("\n");

    /* Query via ioctl */
    struct fb_info info;
    int64_t ret = sys_ioctl(fb, FBIOGET_INFO, (uint64_t)&info);
    if (ret != 0) {
        print_str("  [FAIL] ioctl FBIOGET_INFO returned ");
        print_long(ret);
        print_str("\n");
        sys_close(fb);
        return;
    }
    print_str("  OK: ioctl FBIOGET_INFO succeeded\n");
    print_str("  Resolution: ");
    print_int((int)info.width);
    print_str("x");
    print_int((int)info.height);
    print_str("\n");
    print_str("  Pitch: ");
    print_int((int)info.pitch);
    print_str(" bpp: ");
    print_int(info.bpp);
    print_str("\n");
    print_str("  FB address: ");
    print_hex(info.addr);
    print_str("\n");

    /* Write — clear top-left 100x100 to red */
    if (info.bpp == 32) {
        uint32_t red = 0x00FF0000;  /* ARGB */
        uint32_t line[100];
        for (int x = 0; x < 100; x++) line[x] = red;
        for (int y = 0; y < 100; y++) {
            sys_write(fb, line, 100 * 4);
        }
        print_str("  OK: wrote 100x100 red rectangle\n");

        /* Read back and verify first pixel */
        uint32_t pixel;
        int64_t rd = sys_read(fb, &pixel, 4);
        if (rd == 4) {
            print_str("  Read-back pixel: 0x");
            print_hex(pixel);
            print_str("\n");
            if (pixel == red)
                print_str("  OK: pixel matches red\n");
            else
                print_str("  [WARN] pixel mismatch (may be expected)\n");
        }
    }

    sys_close(fb);
    print_str("  [PASS] framebuffer\n");
}

void test_kbio_events() {
    print_str("\n--- TEST: /dev/kbio (keyboard events) ---\n");

    int kbio = sys_open("/dev/kbio", O_RDONLY, 0);
    if (kbio < 0) {
        print_str("  [FAIL] open /dev/kbio returned ");
        print_long(kbio);
        print_str("\n");
        return;
    }
    print_str("  OK: opened /dev/kbio as fd=");
    print_int(kbio);
    print_str("\n");

    /* Try reading — may return EAGAIN if no keys pressed */
    struct kbio_event evt;
    int64_t rd = sys_read(kbio, &evt, sizeof(evt));
    if (rd > 0) {
        print_str("  OK: got event (");
        print_long(rd);
        print_str(" bytes)\n");
        print_str("  scancode: ");
        print_hex(evt.scancode);
        print_str(" pressed: ");
        print_int(evt.pressed);
        print_str("\n");
    } else {
        print_str("  [INFO] no events pending (EAGAIN = expected)\n");
    }

    sys_close(kbio);
    print_str("  [PASS] kbio\n");
}

void test_mouse_events() {
    print_str("\n--- TEST: /dev/mouse (mouse events) ---\n");

    int mfd = sys_open("/dev/mouse", O_RDONLY, 0);
    if (mfd < 0) {
        print_str("  [FAIL] open /dev/mouse returned ");
        print_long(mfd);
        print_str("\n");
        return;
    }
    print_str("  OK: opened /dev/mouse as fd=");
    print_int(mfd);
    print_str("\n");

    struct mouse_state ms;
    int64_t rd = sys_read(mfd, &ms, sizeof(ms));
    if (rd > 0) {
        print_str("  OK: got state (");
        print_long(rd);
        print_str(" bytes)\n");
        print_str("  dx=");
        print_int(ms.dx);
        print_str(" dy=");
        print_int(ms.dy);
        print_str(" dz=");
        print_int(ms.dz);
        print_str(" btn=0x");
        print_hex(ms.buttons_raw);
        print_str("\n");
    } else {
        print_str("  [INFO] no mouse state change\n");
    }

    sys_close(mfd);
    print_str("  [PASS] mouse\n");
}

void test_mmap_anonymous() {
    print_str("\n--- TEST: anonymous mmap ---\n");
    void *p = sys_mmap(0, 8192, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) { print_str("  [FAIL] mmap returned MAP_FAILED\n"); return; }
    print_str("  OK: got mapping at "); print_hex((uint64_t)p); print_str("\n");

    uint8_t *b = (uint8_t *)p;
    int ok = 1;
    for (int i = 0; i < 8192; i++) { if (b[i] != 0) { ok = 0; break; } }
    if (ok) print_str("  OK: all bytes zeroed\n");

    for (int i = 0; i < 8192; i++) b[i] = (uint8_t)(i & 0xFF);
    ok = 1;
    for (int i = 0; i < 8192; i++) { if (b[i] != (uint8_t)(i & 0xFF)) { ok = 0; break; } }
    if (ok) print_str("  OK: pattern write/read-back passed\n");

    if (sys_munmap(p, 8192) != 0) { print_str("  [FAIL] munmap failed\n"); return; }
    print_str("  OK: munmap succeeded\n");
    print_str("  [PASS] anonymous mmap\n");
}

void test_mmap_mprotect() {
    print_str("\n--- TEST: mprotect ---\n");
    void *p = sys_mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) { print_str("  [FAIL] mmap failed\n"); return; }
    ((uint8_t *)p)[0] = 0xBE;
    if (sys_mprotect(p, 4096, PROT_READ) != 0) { print_str("  [FAIL] mprotect PROT_READ\n"); sys_munmap(p, 4096); return; }
    print_str("  OK: mprotect PROT_READ applied\n");
    if (((uint8_t *)p)[0] != 0xBE) { print_str("  [FAIL] not readable\n"); sys_munmap(p, 4096); return; }
    print_str("  OK: data still readable\n");
    if (sys_mprotect(p, 4096, PROT_READ | PROT_WRITE) != 0) { print_str("  [FAIL] restore\n"); sys_munmap(p, 4096); return; }
    ((uint8_t *)p)[0] = 0xEF;
    if (((uint8_t *)p)[0] != 0xEF) { print_str("  [FAIL] write after restore\n"); sys_munmap(p, 4096); return; }
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
        print_str("  [FAIL] one mapping failed\n");
        if (a != MAP_FAILED) sys_munmap(a, 4096);
        if (b != MAP_FAILED) sys_munmap(b, 8192);
        if (c != MAP_FAILED) sys_munmap(c, 16384);
        return;
    }
    print_str("  OK: 3 mappings created\n");
    if (a == b || b == c || a == c) { print_str("  [FAIL] overlap\n"); }
    else { print_str("  [PASS] distinct addresses\n"); }
    sys_munmap(a, 4096); sys_munmap(b, 8192); sys_munmap(c, 16384);
}

// ============================================================
// Main program
// ============================================================
void test_shared_object();
void main_program(int argc, char **argv) {
    print_str("\n=== Unix Syscall Tests ===\n");
    for(int i = 0; i < argc; i++) {
        print_str("argv["); print_int(i); print_str("] = "); print_str(argv[i]); print_str("\n");
    }

    /* mmap tests */
    test_mmap_anonymous();
    test_mmap_mprotect();
    test_mmap_multi();

    /* Fake VFS tests */
    test_framebuffer();
    test_kbio_events();
    test_mouse_events();
    

    print_str("\n=== All Tests Complete ===\n");
}



void _start(int argc, char **argv, char **envp) {
    const char msg[] = "Hello, DoorsOS! Unix Syscall Test Edition!\n";
    sys_print_write(1, msg, sizeof(msg) - 1);

    print_str("\nArguments:\n");
    print_str("  argc = ");
    print_int(argc);
    print_str("\n");

    print_str("\nEnvironment:\n");
    if (!envp || !envp[0]) { print_str("  <none>\n"); }
    else {
        for (int i = 0; envp[i]; i++) {
            print_str("  envp["); print_int(i); print_str("] = "); print_str(envp[i]); print_str("\n");
        }
    }



    main_program(argc, argv);

    sys_fuck_you();

    print_str("\nDone.\n");
}

// Compile with:
// gcc -nostdlib -nodefaultlibs --save-temps -fPIC -fPIE -Wl,-e,_start -o test_add hello.c
