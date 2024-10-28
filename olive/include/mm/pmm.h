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

#ifndef OLIVE_PMM_H
#define OLIVE_PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

extern uint64_t _hhdm_offset;

#define VIRT_TO_PHYS(vaddr) ((vaddr) - _hhdm_offset)
#define PHYS_TO_VIRT(paddr) ((paddr) + _hhdm_offset)

int pmm_init(void *memmap, uint64_t hhdm_offset);
void *pmm_request_pages(size_t pages);
void pmm_free_pages(void *ptr, size_t pages);

#endif // OLIVE_PMM_H