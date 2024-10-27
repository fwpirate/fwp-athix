#ifndef OLIVE_LIB_PRINTF_H
#define OLIVE_LIB_PRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include <olive.h>

int putchar(char ch);
int puts(const char *str);
int vprintf(const char *fmt, va_list args);
int vfprintf(void *stream, const char *fmt, va_list args);
int printf(const char *fmt, ...);
int fprintf(void *stream, const char *fmt, ...);
int snprintf(char *str, size_t size, const char *fmt, ...);

#endif // OLIVE_LIB_PRINTF_H