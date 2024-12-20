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

#ifndef OLIVE_STDLOCK_H
#define OLIVE_STDLOCK_H

typedef struct
{
    volatile int locked;
} spinlock_t;

#define SPINLOCK_INIT {.locked = 0}

[[maybe_unused]] static void spinlock_acquire(spinlock_t *lock)
{
    while (__sync_lock_test_and_set(&lock->locked, 1))
    {
    }
}

[[maybe_unused]] static void spinlock_release(spinlock_t *lock)
{
    __sync_lock_release(&lock->locked);
}

#endif // OLIVE_STDLOCK_H