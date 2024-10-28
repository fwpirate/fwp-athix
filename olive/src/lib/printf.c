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

#include <printf.h>
#include <stdlock.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW 1
typedef long ssize_t;

#define NANOPRINTF_IMPLEMENTATION
#include <internal/nanoprintf.h>

#include <dev/serial.h>

static spinlock_t printf_lock = SPINLOCK_INIT;

int putchar(char ch)
{
    spinlock_acquire(&printf_lock);

    if (ft_ctx)
    {
        outb(0x3f8, ch); // TODO: Make a proper COM1 handler
        flanterm_write(ft_ctx, &ch, 1);
    }

    spinlock_release(&printf_lock);
    return ch;
}

int puts(const char *str)
{
    spinlock_acquire(&printf_lock);

    int length = 0;
    for (length = 0; *str; length++, putchar(*str++))
        ;
    putchar('\n');

    spinlock_release(&printf_lock);
    return length + 1;
}

int vprintf(const char *fmt, va_list args)
{
    int length = vfprintf(NULL, fmt, args);
    return length;
}

int vfprintf(void *stream, const char *fmt, va_list args)
{
    (void)stream;
    char buffer[1024];

    int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);

    // TODO: Use write function in stream, not yet implemented
    if (length >= 0 && length < (int)sizeof(buffer))
    {
        for (int i = 0; i < length; i++)
        {
            putchar(buffer[i]);
        }
    }

    return length;
}

int printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int length = vprintf(fmt, args);
    va_end(args);
    return length;
}

int fprintf(void *stream, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int length = vfprintf(stream, fmt, args);
    va_end(args);
    return length;
}

int snprintf(char *str, size_t size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int length = npf_vsnprintf(str, size, fmt, args);
    va_end(args);
    return length;
}
