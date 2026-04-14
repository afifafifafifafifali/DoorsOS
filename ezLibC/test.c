#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>

/* Helper: write a string directly to stdout */
static void print(const char* s) {
    write_full(1, s, strlen(s));
}

/* Helper: print a number in decimal */
static void print_num(int64_t n) {
    char buf[32];
    char* p = buf + sizeof(buf) - 1;
    int neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    *p = '\0';
    do { *--p = '0' + (n % 10); n /= 10; } while (n);
    if (neg) *--p = '-';
    write_full(1, p, (size_t)(buf + sizeof(buf) - 1 - p));
}

/* Helper: print hex address */
static void print_hex(uintptr_t v) {
    char buf[20];
    const char* hex = "0123456789abcdef";
    char* p = buf + sizeof(buf) - 1;
    *p = '\0';
    do { *--p = hex[v & 0xf]; v >>= 4; } while (v);
    write_full(1, "0x", 2);
    write_full(1, p, (size_t)(buf + sizeof(buf) - 1 - p));
}

int main(int argc, char* argv[], char* envp[]) {
    (void)argc; (void)argv; (void)envp;

    /* ===== Test strlen, strcpy, memcpy ===== */
    print("=== string/memory tests ===\n");
    const char* hello = "hello world";
    print("strlen(\"hello world\") = ");
    print_num((int64_t)strlen(hello));
    print("\n");

    char buf[64];
    strcpy(buf, "strcpy works!");
    print("strcpy: ");
    print(buf);
    print("\n");

    char dst[32];
    memcpy(dst, "memcpy is great", 16);
    dst[15] = '\0';
    print("memcpy: ");
    print(dst);
    print("\n");

    /* ===== Test sbrk ===== */
    print("\n=== sbrk test ===\n");
    void* old = sbrk(0);
    print("sbrk(0) = ");
    print_hex((uintptr_t)old);
    print("\n");

    void* addr = sbrk(4096);
    print("sbrk(4096) = ");
    print_hex((uintptr_t)addr);
    print("\n");

    /* Write into the sbrk'd memory */
    if (addr != MAP_FAILED) {
        char* p = (char*)addr;
        strcpy(p, "sbrk memory works!");
        print("sbrk content: ");
        print(p);
        print("\n");
    }

    /* ===== Test mmap / munmap ===== */
    print("\n=== mmap/munmap test ===\n");

    void* mapped = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    print("mmap result = ");
    print_hex((uintptr_t)mapped);
    print("\n");

    if (mapped != MAP_FAILED) {
        strcpy((char*)mapped, "mmap memory works!");
        print("mmap content: ");
        print((char*)mapped);
        print("\n");

        /* munmap */
        int ur = munmap(mapped, 4096);
        print("munmap returned: ");
        print_num(ur);
        print("\n");
    } else {
        print("mmap failed!\n");
    }

    /* ===== Test getpid ===== */
    print("\n=== getpid test ===\n");
    print("getpid() = ");
    print_num(getpid());
    print("\n");

    /* ===== Test malloc / free ===== */
    print("\n=== malloc/free test ===\n");

    char* p1 = (char*)malloc(64);
    if (p1) {
        strcpy(p1, "malloc works!");
        print("malloc(64): ");
        print(p1);
        print("\n");
        free(p1);
        print("freed p1\n");
    } else {
        print("malloc failed!\n");
    }

    /* Multiple allocations */
    char* a = (char*)malloc(16);
    char* b = (char*)malloc(32);
    char* c = (char*)malloc(48);
    if (a && b && c) {
        strcpy(a, "block A");
        strcpy(b, "block B");
        strcpy(c, "block C");
        print("a="); print(a); print("\n");
        print("b="); print(b); print("\n");
        print("c="); print(c); print("\n");
    }
    free(c);
    free(a);
    free(b);
    print("freed a, b, c\n");

    /* ===== Test calloc ===== */
    print("\n=== calloc test ===\n");
    int* nums = (int*)calloc(5, sizeof(int));
    if (nums) {
        print("calloc(5, sizeof(int)) zeroed: ");
        for (int i = 0; i < 5; i++) {
            print_num(nums[i]);
            print(" ");
        }
        print("\n");
        nums[0] = 42; nums[4] = 99;
        print("after set: ");
        print_num(nums[0]); print(" ");
        print_num(nums[4]); print("\n");
        free(nums);
    } else {
        print("calloc failed!\n");
    }

    /* ===== Test realloc ===== */
    print("\n=== realloc test ===\n");
    char* rp = (char*)malloc(8);
    if (rp) {
        strcpy(rp, "small");
        print("before realloc: ");
        print(rp);
        print("\n");

        char* rp2 = (char*)realloc(rp, 64);
        if (rp2) {
            print("after realloc: ");
            print(rp2);
            print("\n");
            strcpy(rp2, "realloc grew!");
            print("new content: ");
            print(rp2);
            print("\n");
            free(rp2);
        } else {
            print("realloc failed!\n");
            free(rp);
        }
    }

    /* ===== Test strdup ===== */
    print("\n=== strdup test ===\n");
    char* dup = strdup("hello from strdup");
    if (dup) {
        print("strdup: ");
        print(dup);
        print("\n");
        free(dup);
    }

    print("\nAll tests done!\n");
    return 0;
}
