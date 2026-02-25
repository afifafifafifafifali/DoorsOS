#include "idt.h"
#include "isr.h"

#define GDT_KERNEL_CODE_SELECTOR 0x28

idt_entry_t idt_entries[256];

extern void* isr_stub_table[];
extern void* irq_stub_table[];

idtr_t idtr = {
    sizeof(idt_entries) - 1,
    (uint64_t)&idt_entries
};

void load_idtr(idtr_t* idtr)
{
    asm volatile("lidt %0" : : "m"(*idtr));
}

void set_idt_entry(idt_entry_t* entry,
                   uint64_t offset,
                   uint16_t selector,
                   uint8_t ist,
                   uint8_t type_attributes)
{
    entry->offset_1 = offset & 0xFFFF;
    entry->segment_selector = selector;
    entry->ist = ist;
    entry->type_attributes = type_attributes;
    entry->offset_2 = (offset >> 16) & 0xFFFF;
    entry->offset_3 = (offset >> 32) & 0xFFFFFFFF;
    entry->reserved = 0;
}

void init_idt()
{
    for (uint8_t i = 0; i < 32; i++)
    {
        set_idt_entry(&idt_entries[i],
                      (uint64_t)isr_stub_table[i],
                      GDT_KERNEL_CODE_SELECTOR,
                      0,
                      0x8E);
    }

    for (uint8_t i = 0; i < 16; i++)
    {
        set_idt_entry(&idt_entries[32 + i],
                      (uint64_t)irq_stub_table[i],
                      GDT_KERNEL_CODE_SELECTOR,
                      0,
                      0x8E);
    }

    load_idtr(&idtr);
    enable_interrupts();
}

void enable_interrupts()
{
    asm volatile("sti");
}