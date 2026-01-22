
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

%macro isr_err_stub 1
isr_stub_%+%1:
    push %1        ; interrupt number
    jmp isr_common_stub
%endmacro

%macro isr_no_err_stub 1
isr_stub_%+%1:
    push 0         ; fake error code
    push %1        ; interrupt number
    jmp isr_common_stub
%endmacro

section .text
global isr_stub_table
global irq_stub_table
extern exception_handler
extern irq_handler

isr_common_stub:
    pushad
    cld
    lea rdi, [rsp]       ; pass pointer to interrupt_frame_t
    call exception_handler
    popad
    add rsp, 8           ; remove pushed int_no
    iretq

irq_common_stub:
    pushad
    cld
    lea rdi, [rsp]       ; pass pointer to interrupt_frame_t
    call irq_handler
    popad
    add rsp, 8            ; remove pushed int_no
    iretq

isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

%assign i 32
%rep 16
irq_stub_%+i:
    cli
    push i
    jmp irq_common_stub
    %assign i i+1
%endrep

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
