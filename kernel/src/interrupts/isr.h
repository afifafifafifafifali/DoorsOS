#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Stack layout after our pushes (RSP points to rax at offset 0):
//   [rsp+0]    rax (last pushed by pushad)
//   [rsp+8]    rcx
//   [rsp+16]   rdx
//   [rsp+24]   rdi
//   [rsp+32]   rsi
//   [rsp+40]   r8
//   [rsp+48]   r9
//   [rsp+56]   r10
//   [rsp+64]   r11
//   [rsp+72]   rbx
//   [rsp+80]   rbp
//   [rsp+88]   r12
//   [rsp+96]   r13
//   [rsp+104]  r14
//   [rsp+112]  r15 (first pushed by pushad)
//   [rsp+120]  int_no
//   [rsp+128]  err_code  
//   [rsp+136]  rflags (CPU-pushed)
//   [rsp+144]  cs (CPU-pushed)
//   [rsp+152]  rip (CPU-pushed)

typedef struct interrupt_frame_t {
    uint64_t rax, rcx, rdx, rdi, rsi;
    uint64_t r8, r9, r10, r11;
    uint64_t rbx, rbp, r12, r13, r14, r15;
    uint64_t int_no, err_code;
    uint64_t rflags, cs, rip;
} __attribute__((packed)) interrupt_frame_t;


extern void (*interrupt_handlers[256])(interrupt_frame_t* frame);

void exception_handler(interrupt_frame_t* frame);
void register_irq_handler(uint8_t interrupt, void (*handler)(interrupt_frame_t* frame), char name[100]);
void irq_handler(interrupt_frame_t* frame);

#endif