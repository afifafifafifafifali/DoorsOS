#include "isr.h"
#include "../gfx/printf.h"
#include "../gfx/serial_io.h"
#include "../interrupts/pic.h"

void (*interrupt_handlers[256])(interrupt_frame_t* frame) = {0};

static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating Point",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point",
    "Virtualization",
    "Control Protection",
    "Reserved","Reserved","Reserved","Reserved",
    "Reserved","Reserved","Reserved","Reserved",
    "Reserved","Reserved"
};

// ==========================================
// EXCEPTION HANDLER (0–31)
// ==========================================

void exception_handler(interrupt_frame_t* frame)
{
    uint64_t vec = frame->int_no;

    printf("\n\n=== KERNEL EXCEPTION ===\n");

    if (vec < 32){
        printf("Vector %llu: %s\n", vec, exception_messages[vec]);serial_io_printf("Vector %llu: %s\n", vec, exception_messages[vec]);
    }else{
        printf("Vector %llu: Unknown\n", vec);serial_io_printf("Vector %llu: Unknown\n", vec);}

    printf("RIP:    0x%016llx\n", frame->rip);serial_io_printf("RIP:    0x%016llx\n", frame->rip);
    printf("RSP:    0x%016llx (approx)\n", (uint64_t)frame + sizeof(interrupt_frame_t));serial_io_printf("RSP:    0x%016llx (approx)\n", (uint64_t)frame + sizeof(interrupt_frame_t));
    printf("CS:     0x%016llx\n", frame->cs);serial_io_printf("CS:     0x%016llx\n", frame->cs);
    printf("ERR:    0x%016llx\n", frame->err_code);serial_io_printf("ERR:    0x%016llx\n", frame->err_code);
    printf("RFLAGS: 0x%016llx\n", frame->rflags);serial_io_printf("RFLAGS: 0x%016llx\n", frame->rflags);

    printf("\nRegisters:\n");serial_io_printf("\nRegisters:\n");
    printf("RAX=%016llx RBX=%016llx RCX=%016llx RDX=%016llx\n",
           frame->rax, frame->rbx, frame->rcx, frame->rdx);
    serial_io_printf("RAX=%016llx RBX=%016llx RCX=%016llx RDX=%016llx\n",
           frame->rax, frame->rbx, frame->rcx, frame->rdx);

    printf("RSI=%016llx RDI=%016llx RBP=%016llx\n",
           frame->rsi, frame->rdi, frame->rbp);
    serial_io_printf("RSI=%016llx RDI=%016llx RBP=%016llx\n",
           frame->rsi, frame->rdi, frame->rbp);

    printf("R8 =%016llx R9 =%016llx R10=%016llx R11=%016llx\n",
           frame->r8, frame->r9, frame->r10, frame->r11);
    serial_io_printf("R8 =%016llx R9 =%016llx R10=%016llx R11=%016llx\n",
           frame->r8, frame->r9, frame->r10, frame->r11);

    printf("R12=%016llx R13=%016llx R14=%016llx R15=%016llx\n",
           frame->r12, frame->r13, frame->r14, frame->r15);
    serial_io_printf("R12=%016llx R13=%016llx R14=%016llx R15=%016llx\n",
           frame->r12, frame->r13, frame->r14, frame->r15);

    uint64_t fault_addr;
    __asm__ volatile("mov %%cr2, %0" : "=r"(fault_addr));

    serial_io_printf("CR2:    0x%016llx\n", fault_addr);

    serial_io_printf(".text(current output location counter address)  lives at: 0xffffffff80000000\n");



    printf("\nSystem halted.\n");

    while (1)
        asm volatile("cli; hlt");
}

// ==========================================
// IRQ HANDLER (32–47)
// ==========================================


void irq_handler(interrupt_frame_t* frame)
{
    
    uint8_t vector = (uint8_t)frame->int_no;
    if(vector == 44){
        serial_io_printf("Oh,hi mr mouse\n");
    }
    if (interrupt_handlers[vector])
        interrupt_handlers[vector](frame);
    else
        serial_io_printf("Unhandled IRQ: %u\n", vector);

    uint8_t irq = vector - 32;

    if (irq < 16)
        send_eoi_to_irq(irq);
}


//==========================================
//REGISTER IRQ HANDLER
//==========================================

void register_irq_handler(uint8_t interrupt,
                          void (*handler)(interrupt_frame_t*),
                          char name[100])
{
    interrupt_handlers[interrupt] = handler;
    serial_io_printf("IRQ %u registered by %s\n", interrupt, name);
}