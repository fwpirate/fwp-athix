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

#include <sys/cpu.h>
#include <olive.h>

void hcf(void)
{
    TRACE("Halting CPU and disabling interrupts...\n");
    __asm__ volatile("cli");
    hlt();
}

void hlt(void)
{
    TRACE("Halting CPU...\n");
    for (;;)
    {
        __asm__ volatile(
            "hlt");
    }
}