#pragma once

#include <stdint.h>

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
