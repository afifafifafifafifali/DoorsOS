section .text
global task_switch_asm

; void task_switch_asm(uint64_t* old_rsp, uint64_t new_rsp)
task_switch_asm:
    ; Save current task's context
    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rbp
    push rdi
    push rsi
    push rdx
    push rcx
    push rbx
    push rax
    pushf
    
    ; Save current RSP to old_rsp
    mov [rdi], rsp
    
    ; Load new RSP
    mov rsp, rsi
    
    ; Restore new task's context
    popf
    pop rax
    pop rbx
    pop rcx
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    
    ret
