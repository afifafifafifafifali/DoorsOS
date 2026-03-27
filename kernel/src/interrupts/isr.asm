; ===============================
;  x86_64 ISR + IRQ (0–47)
; ===============================

%macro pushad 0
    push r15
    push r14
    push r13
    push r12
    push rbp
    push rbx
    push r11
    push r10
    push r9
    push r8
    push rsi
    push rdi
    push rdx
    push rcx
    push rax
%endmacro

%macro popad 0
    pop rax
    pop rcx
    pop rdx
    pop rdi
    pop rsi
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbx
    pop rbp
    pop r12
    pop r13
    pop r14
    pop r15
%endmacro

section .text
global isr_stub_table
global irq_stub_table
global syscall_stub

extern exception_handler
extern irq_handler
extern syscall_handler_c

; ==========================================
; Common EXCEPTION stub
; ==========================================

isr_common_stub:
    pushad
    cld
    mov rdi, rsp
    call exception_handler
    popad
    add rsp, 16          ; remove int_no + err_code
    iretq

; ==========================================
; Common IRQ stub (NO error code ever)
; ==========================================

irq_common_stub:
    pushad
    cld
    mov rdi, rsp
    call irq_handler
    popad
    add rsp, 16          ; remove int_no + fake err_code
    iretq

; ==========================================
; Macros
; ==========================================

%macro ISR_ERR 1
isr_stub_%+%1:
    push %1
    jmp isr_common_stub
%endmacro

%macro ISR_NOERR 1
isr_stub_%+%1:
    push 0
    push %1
    jmp isr_common_stub
%endmacro

%macro IRQ_NOERR 1
irq_stub_%+%1:
    push 0       ; fake error code
    push %1      ; interrupt vector
    jmp irq_common_stub
%endmacro

; ==========================================
; Exceptions (0–31)
; ==========================================

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR   30
ISR_NOERR 31

; ==========================================
; IRQs (32–47)
; ==========================================

%assign i 32
%rep 16
IRQ_NOERR i
%assign i i+1
%endrep

; ==========================================
; SYSCALL handler (int 0x80)
; ==========================================
; Syscall convention (6 arguments):
;   rax = syscall number
;   rdi = arg1
;   rsi = arg2
;   rdx = arg3
;   r10 = arg4
;   r8  = arg5
;   r9  = arg6
; Returns: result in rax

syscall_stub:
    push rcx
    push r11
    push r9               ; arg6
    push r8               ; arg5
    push r10              ; arg4
    push rdi              ; arg1
    push rsi              ; arg2
    push rdx              ; arg3
    push rax              ; syscall number

    ; Load syscall number into a temp register BEFORE pushing
    mov r11, [rsp+0]      ; r11 = syscall number

    ; Pass args to C function
    mov rdi, [rsp+24]     ; arg1
    mov rsi, [rsp+16]     ; arg2
    mov rdx, [rsp+8]      ; arg3
    mov rcx, [rsp+32]     ; arg4
    mov r8,  [rsp+40]     ; arg5
    mov r9,  [rsp+48]     ; arg6
    
    ; 7th argument on stack
    push r11              ; syscall number
    call syscall_handler_c
    add rsp, 8

    add rsp, 72
    iretq

; ==========================================
; Tables
; ==========================================

isr_stub_table:
%assign i 0
%rep 32
    dq isr_stub_%+i
%assign i i+1
%endrep

irq_stub_table:
%assign i 32
%rep 16
    dq irq_stub_%+i
%assign i i+1
%endrep