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

extern exception_handler
extern irq_handler

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