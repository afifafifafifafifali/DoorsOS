section .text
global gdt_flush
global tss_flush

gdt_flush:
    lgdt [rdi]          ; Load GDT pointer
    
    ; Reload segment registers
    mov ax, 0x10        ; Kernel data selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Far jump to reload CS
    push 0x08           ; Kernel code selector
    lea rax, [rel .reload_cs]
    push rax
    retfq
    
.reload_cs:
    ret

tss_flush:
    mov ax, 0x28        ; TSS selector
    ltr ax
    ret
