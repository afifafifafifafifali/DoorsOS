#include "isr.h"
#include "../gfx/term.h"
#include "../libs/string.h"
#include "../gfx/printf.h"
#include "../gfx/serial_io.h"
#include "../ps2/io.h"



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
    printf("\n[EXCEPTION] Interrupt: %u (%s)\n", frame->int_no,
           frame->int_no < sizeof(exception_messages)/sizeof(char*) ?
           exception_messages[frame->int_no] : "Unknown");

    if (frame->int_no == 14) {
        // Page Fault: get faulting address
        uint64_t fault_addr;
        asm volatile("mov %%cr2, %0" : "=r"(fault_addr));
        printf("Page Fault at address: 0x%lx\n", fault_addr);
        serial_io_printf("Page Fault at address: 0x%lx\n", fault_addr);
    }

    // Halt the system
    printf("System Halted.\n");
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
