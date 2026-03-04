; Simple x86_64 ELF binary that does 1+1
; Assemble: nasm -f elf64 test_add.S
; Link: ld -n -o test_add test_add.S

bits 64

section .text
global _start

_start:
    ; Do 1+1
    mov rax, 1
    mov rbx, 1
    add rax, rbx        ; rax = 2 now
    
    ; Return back to caller
    ret
