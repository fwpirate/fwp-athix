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