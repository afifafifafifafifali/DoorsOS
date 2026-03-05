; x86_64 ELF binary that does syscall write using int 0x80
; Assemble: nasm -f elf64 test_add.S
; Link: ld -n -o test_add test_add.S

bits 64

section .text
global _start

_start:
    ; syscall: SYS_WRITE (1)
    ; arg1: fd = 1 (stdout)
    ; arg2: buffer pointer (RIP-relative)
    ; arg3: length = 23
    
    lea rsi, [rel message]  ; RIP-relative address of message
    mov rdi, 1              ; stdout
    mov rdx, 23             ; length
    mov rax, 1              ; SYS_WRITE
    int 0x80
    
    ; Return (no exit syscall)
    ret

section .rodata
message: db "Hello from ELF binary!", 10
