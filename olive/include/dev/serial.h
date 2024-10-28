#ifndef OLIVE_SERIAL_H
#define OLIVE_SERIAL_H

#include <stdint.h>
#include <stddef.h>

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);
uint32_t inl(uint16_t port);
void outl(uint16_t port, uint32_t value);

#endif // OLIVE_SERIAL_H
