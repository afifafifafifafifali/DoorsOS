#include "isr.h"
#include "../gfx/term.h"
#include "../libs/string.h"
#include "../gfx/printf.h"
#include "../gfx/serial_io.h"
#include "../ps2/io.h"
#include "../gui/windows.h"
#include "../gui/colorama.h"


// Default exception messages
const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",              // 14
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved"
};



void exception_handler(interrupt_frame_t* frame) {
    char buf[756];

    snprintf(buf, sizeof(buf), "AH SHIT![EXCEPTION] Interrupt: %llu ", frame->int_no);
    kprint_color(buf, COLOR_RGB_RED, true, COLOR_RGB_BLACK, true);
    printf("\n");
    serial_io_printf("%s\n", buf);

    snprintf(buf, sizeof(buf),
        "RAX=0x%016llx RBX=0x%016llx RCX=0x%016llx RDX=0x%016llx\n"
        "RSI=0x%016llx RDI=0x%016llx RBP=0x%016llx RSP=0x%016llx\n"
        "R8 =0x%016llx R9 =0x%016llx R10=0x%016llx R11=0x%016llx\n"
        "R12=0x%016llx R13=0x%016llx R14=0x%016llx R15=0x%016llx\n"
        "ERR=0x%016llx CS=0x%04llx RIP=0x%016llx RFLAGS=0x%016llx\n KERNEL STOP! PANIC",
        frame->rax, frame->rbx, frame->rcx, frame->rdx,
        frame->rsi, frame->rdi, frame->rbp, frame->rsp,
        frame->r8, frame->r9, frame->r10, frame->r11,
        frame->r12, frame->r13, frame->r14, frame->r15,
        frame->err_code, frame->cs, frame->rip, frame->rflags
    );
    kprint_color(buf, COLOR_RGB_RED, true, COLOR_RGB_BLACK, true);
    serial_io_printf("%s\n", buf);

    while (1) asm volatile("cli; hlt");
}


void irq_handler(interrupt_frame_t* frame) {
    if (interrupt_handlers[frame->int_no])
        interrupt_handlers[frame->int_no](frame);
    else
        printf("Unhandled IRQ: %d\n", frame->int_no);
}

void register_irq_handler(uint8_t interrupt, void (*handler)(interrupt_frame_t* frame)) {
    interrupt_handlers[interrupt] = handler;
}
