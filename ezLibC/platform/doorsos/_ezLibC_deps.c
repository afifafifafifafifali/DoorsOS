#include <stdint.h>
#include <_ezLibC_deps.h>
#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2
#define FD_KBIO_EVENTS      3
#define FD_FRAMEBUFFER      4
#define FD_MOUSE_EVENTS     5

  uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2,
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
#define SYS_READ              0
#define SYS_WRITE             1
#define SYS_OPEN              2
#define SYS_CLOSE             3
#define SYS_STAT              4
#define SYS_FSTAT             5
#define SYS_LINK              86

// Process control
#define SYS_EXIT             60
#define SYS_FORK             57
#define SYS_EXECVE           59
#define SYS_WAITPID          61
#define SYS_GETPID           39
#define SYS_TIMES            100

#define SYS_BRK             12

// File operations
#define SYS_LSEEK            49
#define SYS_ACCESS           21
#define SYS_DUP              32
#define SYS_DUP2             33
#define SYS_PIPE             42
#define SYS_GETDENTS68       78

// Time
#define SYS_GETTIMEOFDAY     96
#define SYS_NANOSLEEP        35
#define SYS_CLOCK_GETTIME   228

// SHIT
#define SYS_ARCH_PRCTL      158
#define SYS_UNAME            63


#define SYS_PRINT_WRITE   67671
#define SYS_LOAD_SO       67672
#define SYS_FUCK_YOU      67673

/* Memory mapping — standard Linux x86_64 numbers */
#define SYS_MMAP          9
#define SYS_MPROTECT      10
#define SYS_MUNMAP        11
#define SYS_MSYNC         26



  int64_t syscall0(uint64_t num) {
    return syscall(num, 0, 0, 0, 0, 0, 0);
}

  int64_t syscall1(uint64_t num, uint64_t arg1) {
    return syscall(num, arg1, 0, 0, 0, 0, 0);
}

  int64_t syscall2(uint64_t num, uint64_t arg1, uint64_t arg2) {
    return syscall(num, arg1, arg2, 0, 0, 0, 0);
}

  int64_t syscall3(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    return syscall(num, arg1, arg2, arg3, 0, 0, 0);
}

  int64_t syscall4(uint64_t num, uint64_t arg1, uint64_t arg2,
                               uint64_t arg3, uint64_t arg4) {
    return syscall(num, arg1, arg2, arg3, arg4, 0, 0);
}

  int64_t syscall5(uint64_t num, uint64_t arg1, uint64_t arg2,
                               uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    return syscall(num, arg1, arg2, arg3, arg4, arg5, 0);
}

  int64_t syscall6(uint64_t num, uint64_t arg1, uint64_t arg2,
                               uint64_t arg3, uint64_t arg4, uint64_t arg5,
                               uint64_t arg6) {
    return syscall(num, arg1, arg2, arg3, arg4, arg5, arg6);
}



// Basic I/O
  int64_t sys_read(int fd, void* buf, size_t count) {
    return syscall3(SYS_READ, (uint64_t)fd, (uint64_t)buf, count);
}

  int64_t sys_write(int fd, const void* buf, size_t count) {
    return syscall3(SYS_WRITE, (uint64_t)fd, (uint64_t)buf, count);
}

  int64_t sys_open(const char* pathname, int flags, int mode) {
    return syscall3(SYS_OPEN, (uint64_t)pathname, flags, mode);
}

  int64_t sys_close(int fd) {
    return syscall1(SYS_CLOSE, (uint64_t)fd);
}

  int64_t sys_fstat(int fd, struct stat* st) {
    return syscall2(SYS_FSTAT, (uint64_t)fd, (uint64_t)st);
}

  int64_t sys_stat(const char* path, struct stat* st) {
    return syscall2(SYS_STAT, (uint64_t)path, (uint64_t)st);
}

  int64_t sys_link(const char* oldpath, const char* newpath) {
    return syscall2(SYS_LINK, (uint64_t)oldpath, (uint64_t)newpath);
}

// Process control
  int64_t sys_exit(int status) {
    return syscall1(SYS_EXIT, (uint64_t)status);
}

  int64_t sys_fork(void) {
    return syscall0(SYS_FORK);
}

  int64_t sys_execve(const char* filename, char* const argv[], char* const envp[]) {
    return syscall3(SYS_EXECVE, (uint64_t)filename, (uint64_t)argv, (uint64_t)envp);
}

int64_t int64_isatty(int fd) {
    // Treat standard streams as TTY
    if (fd == STDIN_FILENO ||
        fd == STDOUT_FILENO ||
        fd == STDERR_FILENO) {
        return 1;
    }

    // Keyboard input is also a "terminal-like" device
    if (fd == FD_KBIO_EVENTS) {
        return 1;
    }

    // Everything else → not a tty
    return 0;
}


  int64_t sys_lseek(int fd, off_t offset, int whence) {
    return syscall3(SYS_LSEEK, (uint64_t)fd, (uint64_t)offset, whence);
}

  int64_t sys_dup(int oldfd) {
    return syscall1(SYS_DUP, (uint64_t)oldfd);
}

  int64_t sys_dup2(int oldfd, int newfd) {
    return syscall2(SYS_DUP2, (uint64_t)oldfd, (uint64_t)newfd);
}

  int64_t sys_pipe(int pipefd[2]) {
    return syscall1(SYS_PIPE, (uint64_t)pipefd);
}

// Time
  int64_t sys_gettimeofday(struct timeval* tv, struct timezone* tz) {
    return syscall2(SYS_GETTIMEOFDAY, (uint64_t)tv, (uint64_t)tz);
}

// Misc
  int64_t sys_uname(struct utsname* buf) {
    return syscall1(SYS_UNAME, (uint64_t)buf);
}

  int64_t sys_getpid(void) {
    return syscall0(SYS_GETPID);
}
  int64_t sys_brk(void* addr) {
    return syscall1(SYS_BRK, (uint64_t)addr);
}
#define SYS_IOCTL        16

  int64_t sys_ioctl(int fd, uint64_t req, uint64_t arg) {
    return syscall3(SYS_IOCTL, (uint64_t)fd, req, arg);
}

  void* sys_mmap(void* addr, size_t length, int prot, int flags,
                            int fd, size_t offset) {
    return (void*)syscall6(SYS_MMAP, (uint64_t)addr, length, prot, flags, fd, offset);
}

  int64_t sys_munmap(void* addr, size_t length) {
    return syscall2(SYS_MUNMAP, (uint64_t)addr, length);
}

#define SYS_GETTICKS       686769
int64_t sys_getticks(void){
    return syscall0(SYS_GETTICKS);
}