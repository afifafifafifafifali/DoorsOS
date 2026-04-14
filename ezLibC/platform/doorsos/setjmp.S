/* setjmp.S for ezLibC — x86_64 System V ABI
 *
 * jmp_buf layout (8 × 8-byte values):
 *   [0] rbx   [1] rbp   [2] r12   [3] r13
 *   [4] r14   [5] r15   [6] rsp   [7] rip
 *
 * int __setjmp(jmp_buf env)
 *   — saves callee-saved registers + rsp + rip into env
 *   — returns 0
 *
 * void longjmp(jmp_buf env, int val)
 *   — restores registers from env
 *   — returns val (or 1 if val == 0)
 */

    .global __setjmp
    .global longjmp

/* int __setjmp(jmp_buf env) — env in %rdi */
__setjmp:
    /* Save callee-saved registers */
    mov     %rbx,  0(%rdi)
    mov     %rbp,  8(%rdi)
    mov     %r12, 16(%rdi)
    mov     %r13, 24(%rdi)
    mov     %r14, 32(%rdi)
    mov     %r15, 40(%rdi)

    /* Save rsp (value at call return: rsp + 8 because ret pops rip) */
    lea     8(%rsp), %rax
    mov     %rax, 48(%rdi)

    /* Save rip (return address) */
    mov     0(%rsp), %rax
    mov     %rax, 56(%rdi)

    /* Return 0 */
    xor     %eax, %eax
    ret

/* void longjmp(jmp_buf env, int val) — env in %rdi, val in %esi */
longjmp:
    /* Restore callee-saved registers */
    mov      0(%rdi), %rbx
    mov      8(%rdi), %rbp
    mov     16(%rdi), %r12
    mov     24(%rdi), %r13
    mov     32(%rdi), %r14
    mov     40(%rdi), %r15

    /* Restore rsp */
    mov     48(%rdi), %rsp

    /* Set return value */
    mov     %esi, %eax
    test    %eax, %eax
    jnz     1f
    mov     $1, %eax
1:
    /* Jump to saved rip — push it and ret */
    push    56(%rdi)
    ret
