/*
    Olive the kernel and Athix the OS.
    Copyright (C) 2024  fwpirate <alavik@disroot.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <sys/idt.h>
#include <olive.h>
#include <sys/cpu.h>

idt_entry_t idt_entries[IDT_ENTRY_COUNT];
idt_pointer_t idt_pointer;
extern uint64_t isr_table[];

idt_handler_t irq_handlers[IRQ_COUNT] = {0};

typedef struct
{
    const char *message;
    const char *mnemonic;
    bool fatal;
} exception_info_t;

static const exception_info_t exception_info[32] = {
    {"Division By Zero", "#DE", true},
    {"Debug", "#DB", false},
    {"Non-maskable Interrupt", "-", true},
    {"Breakpoint", "#BP", false},
    {"Overflow", "#OF", true},
    {"Bound Range Exceeded", "#BR", true},
    {"Invalid Opcode", "#UD", true},
    {"Device Not Available", "#NM", true},
    {"Double Fault", "#DF", true},
    {"Coprocessor Segment Overrun", "-", true},
    {"Invalid TSS", "#TS", true},
    {"Segment Not Present", "#NP", true},
    {"Stack Segment Fault", "#SS", true},
    {"General Protection Fault", "#GP", true},
    {"Page Fault", "#PF", true},
    {"Reserved", "-", false},
    {"x87 FPU Error", "#MF", true},
    {"Alignment Check", "#AC", true},
    {"Machine Check", "#MC", true},
    {"SIMD Floating-Point Exception", "#XM/#XF", false},
    {"Virtualization Exception", "#VE", false},
    {"Control Protection Exception", "#CP", false},
    {"Reserved", "-", false},
    {"Reserved", "-", false},
    {"Reserved", "-", false},
    {"Reserved", "-", false},
    {"Reserved", "-", false},
    {"Reserved", "-", false},
    {"Hypervisor Injection Exception", "#HV", false},
    {"VMM Communication Exception", "#VC", false},
    {"Security Exception", "#SX", true},
    {"Reserved", "-", false}};

void idt_set_gate(idt_entry_t idt[], int num, uint64_t base, uint16_t sel, uint8_t flags)
{
    idt[num].offsetLow = (base & 0xFFFF);
    idt[num].offsetMiddle = (base >> 16) & 0xFFFF;
    idt[num].offsetHigh = (base >> 32) & 0xFFFFFFFF;
    idt[num].selector = sel;
    idt[num].ist = 0;
    idt[num].flags = flags;
    idt[num].zero = 0;
    TRACE("Set IDT entry %d: base=0x%lx, selector=0x%x, flags=0x%x\n", num, base, sel, flags);
}

int idt_init()
{
    idt_pointer.limit = sizeof(idt_entry_t) * IDT_ENTRY_COUNT - 1;
    idt_pointer.base = (uintptr_t)&idt_entries;

    for (size_t i = 0; i < IRQ_COUNT; i++)
    {
        irq_handlers[i] = NULL;
    }

    for (int i = 0; i < 32; ++i)
    {
        idt_set_gate(idt_entries, i, isr_table[i], 0x08, 0x8E);
        TRACE("Initialized IDT entry for exception %d (%s): %s\n", i, exception_info[i].mnemonic, exception_info[i].message);
    }

    for (int i = IRQ_BASE; i < IRQ_BASE + IRQ_COUNT; ++i)
    {
        idt_set_gate(idt_entries, i, isr_table[i], 0x08, 0x8E);
        TRACE("Initialized IDT entry for IRQ %d\n", i - IRQ_BASE);
    }

    idt_load((uint64_t)&idt_pointer);
    DEBUG("IDT loaded with base=0x%lx, limit=%u\n", idt_pointer.base, idt_pointer.limit);
    return 0;
}

// TODO: Move
typedef struct stack_frame
{
    struct stack_frame *rbp;
    uint64_t rip;
} stack_frame_t;

void _backtrace(uint64_t caller)
{
    stack_frame_t *frame;
    asm volatile("mov %%rbp, %0" : "=r"(frame));

    FATAL("--- BACKTRACE START ----\n");

    if (frame && frame->rbp == NULL)
    {
        FATAL("No stack trace available-\n");
        return;
    }

    if (caller != 0)
    {
        FATAL(" - [caller] %-24s @ %.16llx\n", "<unkown>", caller);
    }

    int offset = 0;
    for (int i = 0; frame && frame->rbp != NULL; i++)
    {
        FATAL(" - [%06d] %-24s @ %.16llx\n", i - offset, "<unkown>", frame->rip);
        frame = frame->rbp;
    }

    FATAL("-----------------------\n");
}

void idt_handler(int_frame_t frame)
{
    if (frame.vector < 32) // Check for exceptions
    {
        FATAL("\n");
        DEBUG("Received exception %d (%s): %s\n", frame.vector, exception_info[frame.vector].mnemonic, exception_info[frame.vector].message);

        if (exception_info[frame.vector].fatal)
        {
            FATAL("--- INTERRUPT INFORMATION ---\n");
            FATAL(" - %s (%s) @ 0x%.16llx on CPU %s\n", exception_info[frame.vector].mnemonic, exception_info[frame.vector].message, frame.rip, "<unknown>");
            FATAL(" - Register Dump:\n");
            FATAL("\tRAX: 0x%.16llx  RBX:    0x%.16llx  RCX: 0x%.16llx\n", frame.rax, frame.rbx, frame.rcx);
            FATAL("\tRDX: 0x%.16llx  RSI:    0x%.16llx  RDI: 0x%.16llx\n", frame.rdx, frame.rsi, frame.rdi);
            FATAL("\tRBP: 0x%.16llx  RSP:    0x%.16llx  -----------------------\n", frame.rbp, frame.rsp);
            FATAL("\tR8:  0x%.16llx  R9:     0x%.16llx  R10: 0x%.16llx\n", frame.r8, frame.r9, frame.r10);
            FATAL("\tR11: 0x%.16llx  R12:    0x%.16llx  R13: 0x%.16llx\n", frame.r11, frame.r12, frame.r13);
            FATAL("\tR14: 0x%.16llx  R15:    0x%.16llx  -----------------------\n", frame.r14, frame.r15);
            FATAL("\tRIP: 0x%.16llx  RFLAGS: 0x%.16llx  -----------------------\n", frame.rip, frame.rflags);
            FATAL("\tCR2: 0x%.16llx  CR3:    0x%.16llx  -----------------------\n", frame.cr2, frame.cr3);
            FATAL("\tCS:  0x%.16llx  SS:     0x%.16llx  -----------------------\n", frame.cs, frame.ss);
            FATAL("\tERR: 0x%.16llx  VECTOR: 0x%.16llx  -----------------------\n", frame.err, frame.vector);

            switch (frame.vector)
            {
            case 8:
                FATAL(" - Double Fault: No additional error information.\n");
                break;
            case 10:
                FATAL(" - Invalid TSS Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                FATAL("\t - %s\n", (frame.err & 1) ? "External event caused error" : "Descriptor caused error");
                FATAL("\t - Index: 0x%.16llx\n", (frame.err >> 3) & 0x1FFF);
                FATAL("\t - %s\n", (frame.err & (1 << 2)) ? "LDT segment" : "GDT segment");
                break;
            case 11:
                FATAL(" - Segment Not Present Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                FATAL("\t - %s\n", (frame.err & 1) ? "External event caused error" : "Descriptor caused error");
                FATAL("\t - Index: 0x%.16llx\n", (frame.err >> 3) & 0x1FFF);
                FATAL("\t - %s\n", (frame.err & (1 << 2)) ? "LDT segment" : "GDT segment");
                break;
            case 12:
                FATAL(" - Stack-Segment Fault Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                FATAL("\t - %s\n", (frame.err & 1) ? "External event caused error" : "Descriptor caused error");
                FATAL("\t - Index: 0x%.16llx\n", (frame.err >> 3) & 0x1FFF);
                FATAL("\t - %s", (frame.err & (1 << 2)) ? "LDT segment" : "GDT segment");
                break;
            case 13:
                FATAL(" - General Protection Fault Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                FATAL("\t - %s\n", (frame.err & 1) ? "External event caused error" : "Descriptor caused error");
                FATAL("\t - Index: 0x%.16llx\n", (frame.err >> 3) & 0x1FFF);
                FATAL("\t - %s\n", (frame.err & (1 << 2)) ? "LDT segment" : "GDT segment");
                break;
            case 14:
                FATAL(" - Page Fault Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                FATAL("\t - %s\n", (frame.err & 1) ? "Protection violation" : "Non-present page");
                FATAL("\t - %s\n", (frame.err & 2) ? "Write access" : "Read access");
                FATAL("\t - %s\n", (frame.err & 4) ? "User mode" : "Supervisor mode");
                FATAL("\t - %s\n", (frame.err & 8) ? "Reserved bit set" : "No reserved bit violation");
                FATAL("\t - %s\n", (frame.err & 16) ? "Instruction fetch" : "Data access");
                break;
            case 17:
                FATAL(" - Alignment Check Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                FATAL("\t - %s\n", (frame.err & 1) ? "User mode" : "Supervisor mode");
                break;
            case 21:
                FATAL(" - Control Protection Exception Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                break;
            case 29:
                FATAL(" - VMM Communication Exception Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                break;
            case 30:
                FATAL(" - Security Exception Details:\n");
                FATAL("\tError Code: 0x%.16llx\n", frame.err);
                break;
            default:
                FATAL(" - No detailed information for this exception.\n");
                break;
            }
            FATAL("-------------------------------\n");
            _backtrace(frame.rip);
            hcf();
        }
        else
        {
            switch (frame.vector)
            {
            case 3:
                ALERT("BREAKPOINT @ 0x%p\n", frame.rip);
                break;
            default:
                WARN("Non-fatal exception: %s\n", exception_info[frame.vector].message);
                break;
            }
        }
    }
    else if (frame.vector >= IRQ_BASE && frame.vector < IRQ_BASE + IRQ_COUNT) // Check for IRQs
    {
        int irq = frame.vector - IRQ_BASE;
        DEBUG("Received IRQ %d\n", irq);
        if (irq_handlers[irq])
        {
            DEBUG("Invoking handler for IRQ %d\n", irq);
            irq_handlers[irq](&frame);
            DEBUG("Executed IRQ handler for IRQ %d\n", irq);
        }
        else
        {
            WARN("No handler registered for IRQ %d, interrupt ignored\n", irq);
        }
    }
    else if (frame.vector == 0x80) // System call
    {
        WARN("Received system call interrupt, processing skipped\n");
    }
    else // Unknown interrupt
    {
        WARN("Received unknown interrupt vector: %d, no action taken\n", frame.vector);
    }
}

void idt_irq_register(int irq, idt_handler_t handler)
{
    if (irq < 0 || irq >= IRQ_COUNT)
    {
        ERROR("Attempted to register invalid IRQ: %d\n", irq);
        return;
    }
    irq_handlers[irq] = handler;
    DEBUG("Registered IRQ handler for IRQ %d\n", irq);
}

void idt_irq_deregister(int irq)
{
    if (irq < 0 || irq >= IRQ_COUNT)
    {
        ERROR("Attempted to deregister invalid IRQ: %d\n", irq);
        return;
    }
    irq_handlers[irq] = 0;
    DEBUG("Deregistered IRQ handler for IRQ %d\n", irq);
}
