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

#include <mm/pmm.h>
#include <stdlock.h>

static spinlock_t liballoc_lock_var = SPINLOCK_INIT;

int liballoc_lock()
{
    spinlock_acquire(&liballoc_lock_var);
    return 0;
}

int liballoc_unlock()
{
    spinlock_release(&liballoc_lock_var);
    return 0;
}

void *liballoc_alloc(int pages)
{
    return (void *)((uintptr_t)(pmm_request_pages(pages)) + _hhdm_offset);
}

int liballoc_free(void *ptr, int pages)
{
    pmm_free_pages(ptr, pages);
    return 0;
}