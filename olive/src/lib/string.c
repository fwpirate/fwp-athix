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

#include <string.h>

void *memcpy(void *d, const void *s, size_t n)
{
    for (size_t i = 0; i < n; i++)
        ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    return d;
}

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = s;
    while (n--)
        *p++ = (uint8_t)c;
    return s;
}

void *memmove(void *d, const void *s, size_t n)
{
    if (s > d)
        for (size_t i = 0; i < n; i++)
            ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    else
        for (size_t i = n; i-- > 0;)
            ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    return d;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = s1, *p2 = s2;
    while (n--)
    {
        if (*p1 != *p2)
            return (*p1 < *p2) ? -1 : 1;
        p1++;
        p2++;
    }
    return 0;
}