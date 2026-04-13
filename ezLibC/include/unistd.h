/* Full unistd.h for ezLibC — backed only by _ezLibC_deps.h syscalls */

#ifndef __EZLIBC_UNISTD_H
#define __EZLIBC_UNISTD_H 1

#include <_ezLibC_deps.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------
 * Standard file descriptors
 * ------------------------------------------------------------------*/
#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2

/* ------------------------------------------------------------------
 * lseek() whence
 * ------------------------------------------------------------------*/
#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

/* ------------------------------------------------------------------
 * open() flags
 * ------------------------------------------------------------------*/
#define O_RDONLY    00
#define O_WRONLY    01
#define O_RDWR      02
#define O_CREAT     0100
#define O_TRUNC     01000
#define O_APPEND    02000

/* ==================================================================
 * File I/O
 * ================================================================== */

ssize_t read(int fd, void* buf, size_t count);
ssize_t write(int fd, const void* buf, size_t count);
off_t lseek(int fd, off_t offset, int whence);

int open(const char* pathname, int flags, ...);  /* mode needed with O_CREAT */

/* Note: kernel has no close syscall — this is a stub until added */
int close(int fd);

/* ==================================================================
 * Dup / pipe
 * ================================================================== */

int dup(int oldfd);
int dup2(int oldfd, int newfd);
int pipe(int pipefd[2]);

/* ==================================================================
 * Process control
 * ================================================================== */

pid_t fork(void);

int execve(const char* filename, char* const argv[], char* const envp[]);

pid_t getpid(void);

/*
 * _exit  —  calls sys_exit directly, never returns
 * exit   —  same thing (no atexit support yet)
 */
_Noreturn void _exit(int status);
_Noreturn int  exit(int status);

/* ==================================================================
 * File metadata
 * ================================================================== */

int stat(const char* path, struct stat* st);
int fstat(int fd, struct stat* st);
int link(const char* oldpath, const char* newpath);

/* ==================================================================
 * Heap
 * ================================================================== */

void* brk(void* addr);

/*
 * sbrk — relative heap adjust built on sys_brk
 *   increment > 0  → grow heap
 *   increment < 0  → shrink heap
 *   increment == 0 → return current break
 */
void* sbrk(intptr_t increment);

/* ==================================================================
 * Time / system info
 * ================================================================== */

int gettimeofday(struct timeval* tv, struct timezone* tz);
int uname(struct utsname* buf);

/* ==================================================================
 * ioctl
 * ================================================================== */

int ioctl(int fd, uint64_t req, uint64_t arg);

/* ==================================================================
 * Convenience: blocking I/O (retries on partial)
 * ================================================================== */

ssize_t read_full(int fd, void* buf, size_t count);
ssize_t write_full(int fd, const void* buf, size_t count);

#ifdef __cplusplus
}
#endif

#endif
