#ifndef OLIVE_STDLOCK_H
#define OLIVE_STDLOCK_H

// TODO: Move spinlock to elsewhere
typedef struct
{
    volatile int locked;
} spinlock_t;

#define SPINLOCK_INIT {.locked = 0}

static void spinlock_acquire(spinlock_t *lock)
{
    while (__sync_lock_test_and_set(&lock->locked, 1))
    {
    }
}

static void spinlock_release(spinlock_t *lock)
{
    __sync_lock_release(&lock->locked);
}

#endif // OLIVE_STDLOCK_H