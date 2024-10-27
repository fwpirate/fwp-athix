#ifndef OLIVE_IDT_H
#define OLIVE_IDT_H

#include <stdint.h>

#define IDT_ENTRY_COUNT 256
#define IRQ_BASE 0x20
#define IRQ_COUNT 16

typedef struct
{
    uint16_t offsetLow;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t offsetMiddle;
    uint32_t offsetHigh;
    uint32_t zero;
} __attribute__((packed)) idt_entry_t;

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_pointer_t;

typedef struct
{
    uint64_t ds;
    uint64_t cr2;
    uint64_t cr3;

    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    uint64_t vector;
    uint64_t err;

    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;
} __attribute__((packed)) int_frame_t;

extern idt_entry_t idt_entries[IDT_ENTRY_COUNT];

typedef void (*idt_handler_t)(int_frame_t *);
int idt_init();
void idt_load(uint64_t);

void idt_set_gate(idt_entry_t idt[], int num, uint64_t base, uint16_t sel, uint8_t flags);

void idt_irq_register(int irq, idt_handler_t handler);
void idt_irq_deregister(int irq);

#endif // OLIVE_IDT_H