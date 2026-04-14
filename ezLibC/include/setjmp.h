/* Minimal setjmp.h for ezLibC */

#ifndef __EZLIBC_SETJMP_H
#define __EZLIBC_SETJMP_H 1

#ifdef __cplusplus
extern "C" {
#endif

/* x86_64: save rbx, rbp, r12-r15, rsp, rip */
typedef unsigned long __jmp_buf[8];
typedef __jmp_buf jmp_buf;
typedef __jmp_buf sigjmp_buf;

#define setjmp(env) __setjmp(env)

int  __setjmp(jmp_buf env);
_Noreturn void longjmp(jmp_buf env, int val);

/* sigsetjmp / siglongjmp — signal mask not supported */
#define sigsetjmp(env, savemasks) __setjmp(env)
#define siglongjmp(env, val)      longjmp(env, val)

#ifdef __cplusplus
}
#endif

#endif
