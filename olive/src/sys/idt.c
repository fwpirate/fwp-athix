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

void idt_handler(int_frame_t frame)
{
    if (frame.vector < 32) // Check for exceptions
    {
        DEBUG("Received exception %d (%s): %s\n", frame.vector, exception_info[frame.vector].mnemonic, exception_info[frame.vector].message);

        if (exception_info[frame.vector].fatal)
        {
            // TODO: Do a proper panic
            FATAL("Fatal exception: %s (%s), halting cpu.\n", exception_info[frame.vector].message, exception_info[frame.vector].mnemonic);
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
