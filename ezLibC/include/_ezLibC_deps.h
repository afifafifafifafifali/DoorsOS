// borrowed from freestnd-c-headers
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <sys/types.h>

int64_t sys_gettimeofday(struct timeval* tv, struct timezone* tz);
int64_t sys_uname(struct utsname* buf);
int64_t sys_brk(void* addr);
int64_t sys_execve(const char* filename, char* const argv[], char* const envp[]);
int64_t sys_getpid(void);
int64_t sys_fork(void);
int64_t sys_exit(int status);
int64_t sys_link(const char* oldpath, const char* newpath);
int64_t sys_stat(const char* path, struct stat* st);
int64_t sys_fstat(int fd, struct stat* st);
int64_t sys_open(const char* pathname, int flags, int mode);
int64_t sys_write(int fd, const void* buf, size_t count);
int64_t sys_read(int fd, void* buf, size_t count);
int64_t sys_dup2(int oldfd, int newfd);
int64_t sys_dup(int oldfd);
int64_t sys_lseek(int fd, off_t offset, int whence);
int64_t sys_pipe(int pipefd[2]);
int64_t sys_ioctl(int fd, uint64_t req, uint64_t arg);
void* sys_mmap(void* addr, size_t length, int prot, int flags, int fd, size_t offset);
int64_t sys_munmap(void* addr, size_t length);
int64_t sys_close(int fd);
int64_t isatty(int fd);
