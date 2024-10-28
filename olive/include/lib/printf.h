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