#ifndef _ASSERT_H
#define _ASSERT_H

#include <stdio.h>
#ifdef NDEBUG

#define assert(x) ((void)0)

#else

static inline void __assert_fail(const char *expr, const char *file, int line){
    printf("ASSERT FAILED: %s\n", expr);
    printf("FILE: %s\n", file);
    printf("LINE: %d\n", line);
    exit(0);
}

#define assert(x) ((x) ? (void)0 : __assert_fail(#x, __FILE__, __LINE__))

#endif

#endif