#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include "isr.h"
#include <stddef.h>

typedef struct idtr_t {
	uint16_t size;
	uint64_t offset;
}__attribute__((packed)) idtr_t;



typedef struct idt_entry_t {
	uint16_t offset_1;
	uint16_t segment_selector;
	uint8_t ist;
	uint8_t type_attributes;
	uint16_t offset_2;
	uint32_t offset_3;
	uint32_t reserved;
}__attribute__((packed)) idt_entry_t;

// Global IDT entries array (accessible for custom registration)
extern idt_entry_t idt_entries[];

idt_entry_t* load_idt(idtr_t* idtr); // Later implementation
void load_idtr(idtr_t* idtr);
void load_isr(idt_entry_t* entries, uint8_t vec_no, void (*isr)()); // Not yet implemented
void set_idt_entry(idt_entry_t *entry, uint64_t offset, uint16_t selector, uint8_t ist, uint8_t type_attributes);

// Register a custom interrupt handler (for syscalls, etc.)
void register_interrupt_handler(uint8_t vector, void* handler, uint8_t dpl);

void enable_interrupts();
void init_idt();


#endif