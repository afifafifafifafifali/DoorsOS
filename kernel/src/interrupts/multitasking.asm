section .text
global task_switch

; void task_switch(task_t *old, task_t *new)
; rdi = old, rsi = new

%define STACK_SIZE 0x4000

task_switch:
    ; Save context if old != NULL
    cmp rdi, 0
    je .skip_save

    ; Save callee-saved registers to stack
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    ; Save rsp into old->rsp (at offset STACK_SIZE)
    mov [rdi + STACK_SIZE], rsp

.skip_save:
    ; Load new->rsp into rsp (at offset STACK_SIZE)
    mov rsp, [rsi + STACK_SIZE]

    ; Restore callee-saved registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    ret
