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

#include <mm/vmm.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlock.h>
#include <sys/cpu.h>
#include <olive.h>
#include <mm/pmm.h>

pagemap_t _kernel_pm;
extern char __text_start[];
extern char __text_end[];
extern char __rodata_start[];
extern char __rodata_end[];
extern char __data_start[];
extern char __data_end[];

static uintptr_t physical_address_width = 0;
static uintptr_t linear_address_width = 0;

static spinlock_t map_lock = SPINLOCK_INIT;

static uintptr_t *get_below_pml(uintptr_t *pml_pointer, uintptr_t index, bool force);
static uintptr_t *pml4_to_pt(uintptr_t *pml4, uintptr_t virt, bool force);

static inline void tlb_flush()
{
    __asm__ volatile(
        "movq %%cr3, %%rax\n\
        movq %%rax, %%cr3\n"
        : : : "rax");
}

int vmm_init(uintptr_t kernel_virt, uintptr_t kernel_phys)
{
    if (kernel_virt == 0 || kernel_phys == 0)
    {
        ERROR("Kernel virtual or physical addresses are zero, Virtual: 0x%.16llx, Physical: 0x%.16llx\n", kernel_virt, kernel_phys);
        return 1;
    }

    uint32_t eax = 0, ebx = 0, ecx = 0, edx = 0;
    TRACE("Getting CPUID for address widths\n");
    get_cpuid(0x80000008, &eax, &ebx, &ecx, &edx);
    physical_address_width = eax & 0xFF;
    linear_address_width = (eax >> 8) & 0xFF;
    DEBUG("Physical Address Width: %lu / Linear Address Width: %lu\n", physical_address_width, linear_address_width);

    _kernel_pm.pml4 = (uintptr_t)pmm_request_pages(1);
    if (_kernel_pm.pml4 == 0)
    {
        ERROR("Failed to allocate memory for PML4\n");
        return 1;
    }

    _kernel_pm.pml4 += _hhdm_offset;
    TRACE("Kernel PML4: 0x%.16llx\n", _kernel_pm.pml4);
    memset((uint8_t *)_kernel_pm.pml4, 0, PAGE_SIZE);
    TRACE("Zeroed PML4\n");

    uintptr_t text_start = ALIGN_DOWN((uintptr_t)__text_start, PAGE_SIZE),
             rodata_start = ALIGN_DOWN((uintptr_t)__rodata_start, PAGE_SIZE),
             data_start = ALIGN_DOWN((uintptr_t)__data_start, PAGE_SIZE),
             text_end = ALIGN_UP((uintptr_t)__text_end, PAGE_SIZE),
             rodata_end = ALIGN_UP((uintptr_t)__rodata_end, PAGE_SIZE),
             data_end = ALIGN_UP((uintptr_t)__data_end, PAGE_SIZE);

    uintptr_t kernel_base = text_start;
    uintptr_t kernel_highest = (data_end > rodata_end) ? data_end : rodata_end;
    DEBUG("Kernel Base Address: 0x%llx\n", kernel_base);
    DEBUG("Kernel Highest Address: 0x%llx\n", kernel_highest);


    DEBUG("Mapping usable memory and framebuffer\n");
    for (size_t i = 0; i < _memmap->entry_count; i++)
    {
        struct limine_memmap_entry *entry = _memmap->entries[i];

        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            for (size_t off = entry->base; off < entry->base + entry->length; off += PAGE_SIZE)
            {
                vmm_map(&_kernel_pm, off + _hhdm_offset, off, PTE_BIT_PRESENT | PTE_BIT_READ_WRITE);
            }
        }
        else if (entry->type == LIMINE_MEMMAP_FRAMEBUFFER)
        {
            for (size_t off = 0; off < ALIGN_UP(entry->base + entry->length, PAGE_SIZE); off += PAGE_SIZE)
            {
                uintptr_t base_off_aligned = ALIGN_UP(entry->base + off, PAGE_SIZE);
                vmm_map(&_kernel_pm, base_off_aligned + _hhdm_offset, base_off_aligned,
                        PTE_BIT_PRESENT | PTE_BIT_READ_WRITE | PTE_BIT_EXECUTE_DISABLE | PTE_BIT_WRITE_THROUGH_CACHING);
            }
        }
        else if (entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE)
        {
            for (size_t off = 0; off < ALIGN_UP(entry->base + entry->length, PAGE_SIZE); off += PAGE_SIZE)
            {
                vmm_map(&_kernel_pm, entry->base + off + _hhdm_offset, entry->base + off, PTE_BIT_PRESENT | PTE_BIT_READ_WRITE);
            }
        }
    }

    DEBUG("Mapping the kernel text segment\n");
    for (uintptr_t text_addr = text_start; text_addr < text_end; text_addr += PAGE_SIZE)
    {
        uintptr_t phys = text_addr - kernel_virt + kernel_phys;
        vmm_map(&_kernel_pm, text_addr, phys, PTE_BIT_PRESENT);
    }
    DEBUG("Mapping the kernel rodata segment\n");
    for (uintptr_t rodata_addr = rodata_start; rodata_addr < rodata_end; rodata_addr += PAGE_SIZE)
    {
        uintptr_t phys = rodata_addr - kernel_virt + kernel_phys;
        vmm_map(&_kernel_pm, rodata_addr, phys, PTE_BIT_PRESENT | PTE_BIT_EXECUTE_DISABLE);
    }

    DEBUG("Mapping the kernel data segment\n");
    for (uintptr_t data_addr = data_start; data_addr < data_end; data_addr += PAGE_SIZE)
    {
        uintptr_t phys = data_addr - kernel_virt + kernel_phys;
        vmm_map(&_kernel_pm, data_addr, phys, PTE_BIT_PRESENT | PTE_BIT_READ_WRITE | PTE_BIT_EXECUTE_DISABLE);
    }

    DEBUG("Setting the kernel page map into CR3 and flushing it\n");
    vmm_set_pm(&_kernel_pm);
    tlb_flush();

    return 0;
}

static uintptr_t *get_below_pml(uintptr_t *pml_pointer, uintptr_t index, bool force)
{
    if ((pml_pointer[index] & PTE_BIT_PRESENT) != 0)
    {
        return (uintptr_t *)((pml_pointer[index] & 0x000ffffffffff000) + _hhdm_offset);
    }
    if (!force)
    {
        return NULL;
    }

    void *below_pml = pmm_request_pages(1);
    if (below_pml == NULL)
    {
        ERROR("Failed to allocate page for lower level page table\n");
        return NULL;
    }

    memset((void *)((uintptr_t)below_pml + _hhdm_offset), 0, PAGE_SIZE);

    pml_pointer[index] = (uintptr_t)below_pml | PTE_BIT_PRESENT | PTE_BIT_READ_WRITE | PTE_BIT_ACCESS_ALL;
    return (uintptr_t *)((uintptr_t)below_pml + _hhdm_offset);
}

static uintptr_t *pml4_to_pt(uintptr_t *pml4, uintptr_t virt, bool force)
{
    size_t pml4_index = (virt & (0x1fful << 39)) >> 39,
           pdpt_index = (virt & (0x1fful << 30)) >> 30,
           pd_index = (virt & (0x1fful << 21)) >> 21;

    uintptr_t *pdpt = NULL,
             *pd = NULL,
             *pt = NULL;

    pdpt = get_below_pml(pml4, pml4_index, force);
    if (!pdpt)
    {
        return NULL;
    }
    pd = get_below_pml(pdpt, pdpt_index, force);
    if (!pd)
    {
        return NULL;
    }
    pt = get_below_pml(pd, pd_index, force);
    if (!pt)
    {
        return NULL;
    }

    return pt;
}

void vmm_map(pagemap_t *pm, uintptr_t virt, uintptr_t phys, uint64_t flagsg)
{
    spinlock_acquire(&map_lock);
    size_t pt_index = (virt & (0x1fful << 12)) >> 12;
    uintptr_t *pt = pml4_to_pt((uintptr_t *)pm->pml4, virt, true);
    if (pt == NULL)
    {
        ERROR("Page table doesn't exist and didn't get created?");
        spinlock_release(&map_lock);
        return;
    }
    pt[pt_index] = phys | flags;

    tlb_flush();
    spinlock_release(&map_lock);
}

bool vmm_unmap(pagemap_t *pm, uintptr_t virt, bool free_phys)
{
    spinlock_acquire(&map_lock);
    size_t pt_index = (virt & (0x1fful << 12)) >> 12;
    uintptr_t *pt = pml4_to_pt((uintptr_t *)pm->pml4, virt, false);
    if (pt == NULL)
    {
        spinlock_release(&map_lock);
        return false;
    }

    if (free_phys)
    {
        pmm_free_pages((void *)(pt[pt_index] & 0x000ffffffffff000), 1);
    }

    pt[pt_index] = 0;

    tlb_flush();
    spinlock_release(&map_lock);

    return true;
}

uintptr_t virt2phys(pagemap_t *pm, uintptr_t virt)
{
    size_t pt_index = (virt & (0x1fful << 12)) >> 12;
    uintptr_t *pt = pml4_to_pt((uintptr_t *)pm->pml4, virt, false);
    if (!pt)
        return (uintptr_t)NULL;
    uintptr_t mask = ((1ull << physical_address_width) - 1) & ~0xFFF;

    return (pt[pt_index] & mask) | (virt & 0xFFF);
}

void vmm_set_pm(pagemap_t *pm)
{
    __asm__ volatile(
        "movq %0, %%cr3\n"
        : : "r"((uintptr_t *)(pm->pml4 - _hhdm_offset)) : "memory");
}
