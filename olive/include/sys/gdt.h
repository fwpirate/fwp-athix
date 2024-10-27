#ifndef OLIVE_GDT_H
#define OLIVE_GDT_H

#include <stdint.h>

typedef struct
{
    uint16_t limit;
    uintptr_t base;
} __attribute((packed)) gdt_pointer_t;

typedef struct
{
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t accessByte;
    uint8_t limitHighAndFlags;
    uint8_t baseHigh;
} __attribute((packed)) gdt_entry_t;

extern gdt_pointer_t gdtr;

int gdt_init();
void gdt_flush(gdt_pointer_t gdtr);

#endif // OLIVE_GDT_H