#ifndef OLIVE_STRING_H
#define OLIVE_STRING_H

#include <stdint.h>
#include <stddef.h>

void *memcpy(void *d, const void *s, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *d, const void *s, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif // OLIVE_STRING_H