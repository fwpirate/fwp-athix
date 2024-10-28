#include <sys/gdt.h>
#include <olive.h>

gdt_pointer_t gdt_pointer;
gdt_entry_t gdt_entries[5];

int gdt_init()
{
    // Initialize GDT entries
    gdt_entries[0] = (gdt_entry_t){0, 0, 0, 0, 0, 0};                   // Null segment
    gdt_entries[1] = (gdt_entry_t){0, 0, 0, 0b10011010, 0b10100000, 0}; // Code segment
    gdt_entries[2] = (gdt_entry_t){0, 0, 0, 0b10010010, 0b10100000, 0}; // Data segment
    gdt_entries[3] = (gdt_entry_t){0, 0, 0, 0b11111010, 0b10100000, 0}; // User code segment
    gdt_entries[4] = (gdt_entry_t){0, 0, 0, 0b11110010, 0b10100000, 0}; // User data segment

    gdt_pointer.limit = (uint16_t)(sizeof(gdt_entries) - 1);
    gdt_pointer.base = (uintptr_t)&gdt_entries;

    DEBUG("GDT initialized with base=0x%lx, limit=%u\n", gdt_pointer.base, gdt_pointer.limit);

    gdt_flush(gdt_pointer);
    return 0;
}

void gdt_flush(gdt_pointer_t gdtr)
{
    TRACE("Flushing GDT...\n");

    __asm__ volatile(
        "mov %0, %%rdi\n"
        "lgdt (%%rdi)\n"
        "push $0x8\n"
        "lea 1f(%%rip), %%rax\n"
        "push %%rax\n"
        "lretq\n"
        "1:\n"
        "mov $0x10, %%ax\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%ss\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%fs\n"
        :
        : "r"(&gdtr)
        : "memory");

    TRACE("GDT flushed successfully.\n");
    TRACE("Segment selectors set to 0x10 for all segments.\n");
}
