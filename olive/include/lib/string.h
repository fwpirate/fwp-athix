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

#ifndef OLIVE_STRING_H
#define OLIVE_STRING_H

#include <stdint.h>
#include <stddef.h>

void *memcpy(void *d, const void *s, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *d, const void *s, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif // OLIVE_STRING_H