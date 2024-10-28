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