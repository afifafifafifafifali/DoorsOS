#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stddef.h>



typedef int64_t off_t;
typedef int64_t pid_t;
typedef int64_t ssize_t;
typedef uint64_t size_t;
typedef int64_t clock_t;


struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
};

struct timeval {
    int64_t tv_sec;
    int64_t tv_usec;
};

struct timezone {
    int32_t tz_minuteswest;
    int32_t tz_dsttime;
};

struct timespec {
    int64_t tv_sec;
    int64_t tv_nsec;
};

struct tms {
    int64_t tms_utime;  /* user time */
    int64_t tms_stime;  /* system time */
    int64_t tms_cutime; /* user time of children */
    int64_t tms_cstime; /* system time of children */
};

struct stat {
    uint64_t st_dev;     /* ID of device containing file */
    uint64_t st_ino;     /* inode number */
    uint32_t st_mode;    /* protection */
    uint32_t st_nlink;   /* number of hard links */
    uint32_t st_uid;     /* user ID of owner */
    uint32_t st_gid;     /* group ID of owner */
    uint64_t st_rdev;    /* device ID (if special file) */
    uint64_t st_size;    /* total size, in bytes */
    uint64_t st_blksize; /* blocksize for filesystem I/O */
    uint64_t st_blocks;  /* number of 512B blocks allocated */
    int64_t  st_atime;   /* time of last access */
    int64_t  st_mtime;   /* time of last modification */
    int64_t  st_ctime;   /* time of last status change */
};



// Basic I/O
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



// Open flags
#define O_RDONLY    0x0001
#define O_WRONLY    0x0002
#define O_RDWR      0x0003
#define O_CREAT     0x0100
#define O_TRUNC     0x0200
#define O_APPEND    0x0400

// seek 
#define SEEK_SET            0
#define SEEK_CUR            1
#define SEEK_END            2

// stat mode bits
#define S_IFMT   0170000   /* type of file mask */
#define S_IFREG  0100000   /* regular file */
#define S_IFDIR  0040000   /* directory */
#define S_IFCHR  0020000   /* character special */
#define S_IFBLK  0060000   /* block special */
#define S_IFIFO  0010000   /* FIFO special */
#define S_IFLNK  0120000   /* symbolic link */

#define S_IRUSR  0000400   /* read permission, owner */
#define S_IWUSR  0000200   /* write permission, owner */
#define S_IXUSR  0000100   /* execute/search permission, owner */



// Error codes (errno values)
#define E_SUCCESS           0
#define EPERM               1
#define ENOENT              2
#define ESRCH               3
#define EINTR               4
#define EIO                 5
#define ENXIO               6
#define E2BIG               7
#define ENOEXEC             8
#define EBADF               9
#define ECHILD             10
#define EAGAIN             11
#define ENOMEM             12
#define EACCES             13
#define EFAULT             14
#define ENOTBLK            15
#define EBUSY              16
#define EEXIST             17
#define EXDEV              18
#define ENODEV             19
#define ENOTDIR            20
#define EISDIR             21
#define EINVAL             22
#define ENFILE             23
#define EMFILE             24
#define ENOTTY             25
#define ETXTBSY            26
#define EFBIG              27
#define ENOSPC             28
#define ESPIPE             29
#define EROFS              30
#define EMLINK             31
#define EPIPE              32
#define EDOM               33
#define ERANGE             34
#define ENOSYS             38
#define EDEADLK            35
#define ENAMETOOLONG       36
#define ENOLCK             37

// Standard file descriptors
#define STDIN_FILENO        0
#define STDOUT_FILENO       1
#define STDERR_FILENO       2

/* Special DoorsOS pseudo-FDs (returned by open on fake paths) */
#define FD_KBIO_EVENTS      3
#define FD_FRAMEBUFFER      4
#define FD_MOUSE_EVENTS     5

/* Framebuffer info returned via ioctl() */
#pragma pack(push, 1)
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
};
#pragma pack(pop)

/* ioctl commands */
#define FBIOGET_INFO     0x4601  /* get fb_info struct */
#define KBIO_GET_MODE    0x4B01  /* get kbio mode */
#define KBIO_SET_MODE    0x4B02  /* set kbio mode */

/* ioctl syscall number (standard x86_64 Linux) */
#define SYS_IOCTL        16

// Exit status macros
#define WEXITSTATUS(status) (((status) >> 8) & 0xFF)
#define WTERMSIG(status)    ((status) & 0x7F)
#define WIFEXITED(status)   (WTERMSIG(status) == 0)



static inline int64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2,
                              uint64_t arg3, uint64_t arg4, uint64_t arg5,
                              uint64_t arg6) {
    int64_t ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3),
          "r"((uint64_t)arg4), "r"((uint64_t)arg5), "r"((uint64_t)arg6)
        : "rcx", "r11", "memory", "cc"
    );
    return ret;
}


static inline int64_t syscall0(uint64_t num) {
    return syscall(num, 0, 0, 0, 0, 0, 0);
}

static inline int64_t syscall1(uint64_t num, uint64_t arg1) {
    return syscall(num, arg1, 0, 0, 0, 0, 0);
}

static inline int64_t syscall2(uint64_t num, uint64_t arg1, uint64_t arg2) {
    return syscall(num, arg1, arg2, 0, 0, 0, 0);
}

static inline int64_t syscall3(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    return syscall(num, arg1, arg2, arg3, 0, 0, 0);
}

static inline int64_t syscall4(uint64_t num, uint64_t arg1, uint64_t arg2,
                               uint64_t arg3, uint64_t arg4) {
    return syscall(num, arg1, arg2, arg3, arg4, 0, 0);
}

static inline int64_t syscall5(uint64_t num, uint64_t arg1, uint64_t arg2,
                               uint64_t arg3, uint64_t arg4, uint64_t arg5) {
    return syscall(num, arg1, arg2, arg3, arg4, arg5, 0);
}

static inline int64_t syscall6(uint64_t num, uint64_t arg1, uint64_t arg2,
                               uint64_t arg3, uint64_t arg4, uint64_t arg5,
                               uint64_t arg6) {
    return syscall(num, arg1, arg2, arg3, arg4, arg5, arg6);
}



// Basic I/O
static inline int64_t sys_read(int fd, void* buf, size_t count) {
    return syscall3(SYS_READ, (uint64_t)fd, (uint64_t)buf, count);
}

static inline int64_t sys_write(int fd, const void* buf, size_t count) {
    return syscall3(SYS_WRITE, (uint64_t)fd, (uint64_t)buf, count);
}

static inline int64_t sys_open(const char* pathname, int flags, int mode) {
    return syscall3(SYS_OPEN, (uint64_t)pathname, flags, mode);
}

static inline int64_t sys_close(int fd) {
    return syscall1(SYS_CLOSE, (uint64_t)fd);
}

static inline int64_t sys_fstat(int fd, struct stat* st) {
    return syscall2(SYS_FSTAT, (uint64_t)fd, (uint64_t)st);
}

static inline int64_t sys_stat(const char* path, struct stat* st) {
    return syscall2(SYS_STAT, (uint64_t)path, (uint64_t)st);
}

static inline int64_t sys_link(const char* oldpath, const char* newpath) {
    return syscall2(SYS_LINK, (uint64_t)oldpath, (uint64_t)newpath);
}

// Process control
static inline int64_t sys_exit(int status) {
    return syscall1(SYS_EXIT, (uint64_t)status);
}

static inline int64_t sys_fork(void) {
    return syscall0(SYS_FORK);
}

static inline int64_t sys_execve(const char* filename, char* const argv[], char* const envp[]) {
    return syscall3(SYS_EXECVE, (uint64_t)filename, (uint64_t)argv, (uint64_t)envp);
}

static inline int64_t sys_waitpid(int pid, int* status, int options) {
    return syscall3(SYS_WAITPID, (uint64_t)pid, (uint64_t)status, options);
}

static inline int64_t sys_getpid(void) {
    return syscall0(SYS_GETPID);
}

static inline clock_t sys_times(struct tms* buf) {
    return (clock_t)syscall1(SYS_TIMES, (uint64_t)buf);
}



static inline int64_t sys_brk(void* addr) {
    return syscall1(SYS_BRK, (uint64_t)addr);
}

// File operations
static inline int64_t sys_lseek(int fd, off_t offset, int whence) {
    return syscall3(SYS_LSEEK, (uint64_t)fd, (uint64_t)offset, whence);
}

static inline int64_t sys_dup(int oldfd) {
    return syscall1(SYS_DUP, (uint64_t)oldfd);
}

static inline int64_t sys_dup2(int oldfd, int newfd) {
    return syscall2(SYS_DUP2, (uint64_t)oldfd, (uint64_t)newfd);
}

static inline int64_t sys_pipe(int pipefd[2]) {
    return syscall1(SYS_PIPE, (uint64_t)pipefd);
}

// Time
static inline int64_t sys_gettimeofday(struct timeval* tv, struct timezone* tz) {
    return syscall2(SYS_GETTIMEOFDAY, (uint64_t)tv, (uint64_t)tz);
}

// Misc
static inline int64_t sys_uname(struct utsname* buf) {
    return syscall1(SYS_UNAME, (uint64_t)buf);
}

// Legacy wrappers (for backwards compatibility)
static inline int64_t sys_print(const char* buf, size_t count) {
    return syscall3(SYS_PRINT_WRITE, 1, (uint64_t)buf, count);
}

static inline uint64_t sys_print_write(int fd, const char* buf, uint64_t count) {
    return syscall3(SYS_PRINT_WRITE, (uint64_t)fd, (uint64_t)buf, count);
}

static inline int64_t sys_fuck_you(void) {
    return syscall(SYS_FUCK_YOU, 11, 22, 33, 44, 55, 66);
}

/* Memory mapping syscalls */
static inline void* sys_mmap(void* addr, size_t length, int prot, int flags,
                              int fd, size_t offset) {
    return (void*)syscall6(SYS_MMAP, (uint64_t)addr, length, prot, flags, fd, offset);
}

static inline int64_t sys_munmap(void* addr, size_t length) {
    return syscall2(SYS_MUNMAP, (uint64_t)addr, length);
}

static inline int64_t sys_msync(void* addr, size_t length) {
    return syscall2(SYS_MSYNC, (uint64_t)addr, length);
}

static inline int64_t sys_mprotect(void* addr, size_t length, int prot) {
    return syscall3(SYS_MPROTECT, (uint64_t)addr, length, prot);
}

/* ioctl */
static inline int64_t sys_ioctl(int fd, uint64_t req, uint64_t arg) {
    return syscall3(SYS_IOCTL, (uint64_t)fd, req, arg);
}

/* Fake VFS convenience wrappers */
static inline int sys_open_kbio(void) {
    return sys_open("/dev/kbio", O_RDONLY, 0);
}

static inline int sys_open_framebuffer(void) {
    return sys_open("/dev/fb0", O_RDWR, 0);
}

static inline int sys_open_mouse(void) {
    return sys_open("/dev/mouse", O_RDONLY, 0);
}


void syscall_init(void);

uint64_t syscall_handler_c(uint64_t arg1, uint64_t arg2, uint64_t arg3,
                           uint64_t arg4, uint64_t arg5, uint64_t arg6,
                           uint64_t num);



#endif