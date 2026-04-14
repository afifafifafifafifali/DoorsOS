#include <unistd.h>
#include <stdarg.h>

/* ==================================================================
 * File I/O
 * ================================================================== */

ssize_t read(int fd, void* buf, size_t count) {
    return (ssize_t)sys_read(fd, buf, count);
}

ssize_t write(int fd, const void* buf, size_t count) {
    return (ssize_t)sys_write(fd, buf, count);
}

off_t lseek(int fd, off_t offset, int whence) {
    return (off_t)sys_lseek(fd, offset, whence);
}

int open(const char* pathname, int flags, ...) {
    va_list ap;
    va_start(ap, flags);
    int mode = va_arg(ap, int);
    va_end(ap);
    return (int)sys_open(pathname, flags, mode);
}

int close(int fd) {
   return sys_close(fd);
}

/* ==================================================================
 * Dup / pipe
 * ================================================================== */

int dup(int oldfd) {
    return (int)sys_dup(oldfd);
}

int dup2(int oldfd, int newfd) {
    return (int)sys_dup2(oldfd, newfd);
}

int pipe(int pipefd[2]) {
    return (int)sys_pipe(pipefd);
}

/* ==================================================================
 * Process control
 * ================================================================== */

pid_t fork(void) {
    return (pid_t)sys_fork();
}

int execve(const char* filename, char* const argv[], char* const envp[]) {
    return (int)sys_execve(filename, argv, envp);
}

pid_t getpid(void) {
    return (pid_t)sys_getpid();
}

   void _exit(int status) {
    sys_exit(status);
    for (;;);
}

   int exit(int status) {
    _exit(status);
}

/* ==================================================================
 * File metadata
 * ================================================================== */

int stat(const char* path, struct stat* st) {
    return (int)sys_stat(path, st);
}

int fstat(int fd, struct stat* st) {
    return (int)sys_fstat(fd, st);
}

int link(const char* oldpath, const char* newpath) {
    return (int)sys_link(oldpath, newpath);
}

/* ==================================================================
 * Heap
 * ================================================================== */

void* brk(void* addr) {
    return (void*)(intptr_t)sys_brk(addr);
}

void* sbrk(intptr_t increment) {
    static void* cur_brk = 0;
    static int initialized = 0;

    if (!initialized) {
        cur_brk = (void*)(intptr_t)sys_brk(0);
        initialized = 1;
    }

    void* prev = cur_brk;

    /* Round up to page boundary */
    intptr_t pagesize = 4096;
    intptr_t rounded = (increment + pagesize - 1) & ~(pagesize - 1);

    void* new_brk = (void*)((char*)cur_brk + rounded);
    if (sys_brk(new_brk) == (int64_t)-1) {
        return (void*)-1;
    }

    cur_brk = new_brk;
    return prev;
}

/* ==================================================================
 * Time / system info
 * ================================================================== */

int gettimeofday(struct timeval* tv, struct timezone* tz) {
    return (int)sys_gettimeofday(tv, tz);
}

int uname(struct utsname* buf) {
    return (int)sys_uname(buf);
}

/* ==================================================================
 * ioctl
 * ================================================================== */

int ioctl(int fd, uint64_t req, uint64_t arg) {
    return (int)sys_ioctl(fd, req, arg);
}

/* ==================================================================
 * Convenience: blocking I/O
 * ================================================================== */

ssize_t read_full(int fd, void* buf, size_t count) {
    ssize_t total = 0;
    char* p = (char*)buf;
    while (count > 0) {
        ssize_t n = read(fd, p, count);
        if (n <= 0) return total > 0 ? total : n;
        p += n;
        count -= n;
        total += n;
    }
    return total;
}

ssize_t write_full(int fd, const void* buf, size_t count) {
    ssize_t total = 0;
    const char* p = (const char*)buf;
    while (count > 0) {
        ssize_t n = write(fd, p, count);
        if (n <= 0) return total > 0 ? total : n;
        p += n;
        count -= n;
        total += n;
    }
    return total;
}
