/* Linux syscall implementations for ezLibC */

#include <_ezLibC_deps.h>

/* Raw x86_64 syscall instruction */

static long syscall0(long n) {
    long ret;
    __asm__ volatile("syscall" : "=a"(ret) : "a"(n) : "rcx", "r11", "memory");
    return ret;
}

static long syscall1(long n, long a1) {
    long ret;
    __asm__ volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1) : "rcx", "r11", "memory");
    return ret;
}

static long syscall2(long n, long a1, long a2) {
    long ret;
    __asm__ volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2) : "rcx", "r11", "memory");
    return ret;
}

static long syscall3(long n, long a1, long a2, long a3) {
    long ret;
    __asm__ volatile("syscall" : "=a"(ret) : "a"(n), "D"(a1), "S"(a2), "d"(a3) : "rcx", "r11", "memory");
    return ret;
}

static long syscall4(long n, long a1, long a2, long a3, long a4) {
    long ret;
    __asm__ volatile("movq %5, %%r10\n\t"
                     "syscall"
                     : "=a"(ret)
                     : "a"(n), "D"(a1), "S"(a2), "d"(a3), "g"(a4)
                     : "rcx", "r11", "r10", "memory");
    return ret;
}

static long syscall5(long n, long a1, long a2, long a3, long a4, long a5) {
    long ret;
    __asm__ volatile("movq %5, %%r10\n\t"
                     "movq %6, %%r8\n\t"
                     "syscall"
                     : "=a"(ret)
                     : "a"(n), "D"(a1), "S"(a2), "d"(a3), "g"(a4), "g"(a5)
                     : "rcx", "r11", "r10", "r8", "memory");
    return ret;
}

static long syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6) {
    long ret;
    __asm__ volatile("movq %5, %%r10\n\t"
                     "movq %6, %%r8\n\t"
                     "movq %7, %%r9\n\t"
                     "syscall"
                     : "=a"(ret)
                     : "a"(n), "D"(a1), "S"(a2), "d"(a3), "g"(a4), "g"(a5), "g"(a6)
                     : "rcx", "r11", "r10", "r8", "r9", "memory");
    return ret;
}

/* Linux syscall numbers x86_64 */
#define __NR_read            0
#define __NR_write           1
#define __NR_open            2
#define __NR_close           3
#define __NR_getpid         39
#define __NR_fork           57
#define __NR_execve         59
#define __NR_exit           60
#define __NR_dup            32
#define __NR_dup2           33
#define __NR_pipe           22
#define __NR_lseek           8
#define __NR_brk            12
#define __NR_mmap            9
#define __NR_munmap         11
#define __NR_ioctl          16
#define __NR_link           86
#define __NR_gettimeofday   96
#define __NR_uname          63
#define __NR_fstat           5
#define __NR_newfstatat    262

int64_t sys_gettimeofday(struct timeval* tv, struct timezone* tz) {
    return (int64_t)syscall2(__NR_gettimeofday, (long)tv, (long)tz);
}

int64_t sys_uname(struct utsname* buf) {
    return (int64_t)syscall1(__NR_uname, (long)buf);
}

int64_t sys_brk(void* addr) {
    return (int64_t)syscall1(__NR_brk, (long)addr);
}

int64_t sys_execve(const char* filename, char* const argv[], char* const envp[]) {
    return (int64_t)syscall3(__NR_execve, (long)filename, (long)argv, (long)envp);
}

int64_t sys_getpid(void) {
    return (int64_t)syscall0(__NR_getpid);
}

int64_t sys_fork(void) {
    return (int64_t)syscall0(__NR_fork);
}

int64_t sys_exit(int status) {
    return (int64_t)syscall1(__NR_exit, (long)status);
}

int64_t sys_link(const char* oldpath, const char* newpath) {
    return (int64_t)syscall2(__NR_link, (long)oldpath, (long)newpath);
}

int64_t sys_stat(const char* path, struct stat* st) {
    return (int64_t)syscall4(__NR_newfstatat, -100, (long)path, (long)st, 0);
}

int64_t sys_fstat(int fd, struct stat* st) {
    return (int64_t)syscall3(__NR_fstat, fd, (long)st, 0);
}

int64_t sys_open(const char* pathname, int flags, int mode) {
    return (int64_t)syscall3(__NR_open, (long)pathname, (long)flags, (long)mode);
}

int64_t sys_write(int fd, const void* buf, size_t count) {
    return (int64_t)syscall3(__NR_write, fd, (long)buf, (long)count);
}

int64_t sys_read(int fd, void* buf, size_t count) {
    return (int64_t)syscall3(__NR_read, fd, (long)buf, (long)count);
}

int64_t sys_dup2(int oldfd, int newfd) {
    return (int64_t)syscall2(__NR_dup2, oldfd, (long)newfd);
}

int64_t sys_dup(int oldfd) {
    return (int64_t)syscall1(__NR_dup, (long)oldfd);
}

int64_t sys_lseek(int fd, off_t offset, int whence) {
    return (int64_t)syscall3(__NR_lseek, fd, (long)offset, (long)whence);
}

int64_t sys_pipe(int pipefd[2]) {
    return (int64_t)syscall1(__NR_pipe, (long)pipefd);
}

int64_t sys_ioctl(int fd, uint64_t req, uint64_t arg) {
    return (int64_t)syscall3(__NR_ioctl, fd, (long)req, (long)arg);
}

void* sys_mmap(void* addr, size_t length, int prot, int flags, int fd, size_t offset) {
    return (void*)syscall6(__NR_mmap, (long)addr, (long)length, (long)prot, (long)flags, (long)fd, (long)offset);
}

int64_t sys_munmap(void* addr, size_t length) {
    return (int64_t)syscall2(__NR_munmap, (long)addr, (long)length);
}

int64_t sys_close(int fd) {
    return (int64_t)syscall1(__NR_close, (long)fd);
}

/* Linux has no isatty syscall — implemented via ioctl(TCGETS) */
#define __TCGETS 0x5401

int64_t isatty(int fd) {
    char buf[64];
    long ret = syscall3(__NR_ioctl, fd, __TCGETS, (long)buf);
    return ret == 0 ? 1 : 0;
}
