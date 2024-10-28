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

#ifndef OLIVE_VMM_H
#define OLIVE_VMM_H

#include <stdint.h>
#include <stdlib.h>
#include <mm/pmm.h>
#include <stdbool.h>

typedef struct pagemap
{
    uintptr_t pml4;
} pagemap_t;

#define PTE_BIT_PRESENT 1ul << 0
#define PTE_BIT_READ_WRITE 1ul << 1
#define PTE_BIT_ACCESS_ALL 1ul << 2
#define PTE_BIT_WRITE_THROUGH_CACHING 1ul << 3
#define PTE_BIT_DISABLE_CACHING 1ul << 4
#define PTE_BIT_PDE_OR_PTE_ACCESSED 1ul << 5
#define PTE_BIT_DIRTY 1ul << 6
#define PTE_BIT_PAT_SUPPORTED 1ul << 7
#define PTE_BIT_GLOBAL 1ul << 8
#define PTE_BIT_EXECUTE_DISABLE 1ul << 63

extern pagemap_t _kernel_pm;

int vmm_init(uintptr_t kernel_virt, uintptr_t kernel_phys);
void vmm_set_pm(pagemap_t *pm);
void vmm_map(pagemap_t *pm, uintptr_t virt, uintptr_t phys, uint64_t flags);
bool vmm_unmap(pagemap_t *pm, uintptr_t virt, bool free_phys);
uintptr_t vmm_virt2phys(pagemap_t *pm, uintptr_t virt);

#endif // OLIVE_VMM_H