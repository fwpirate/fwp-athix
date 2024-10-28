#include <mm/pmm.h>
#include <olive.h>
#include <string.h>

uint64_t *pm_stack;
uint64_t pm_stack_idx = 0;
uint64_t pm_stack_count = 0;
uint64_t pm_free_pages = 0;
uint64_t pm_used_pages = 0;
uint64_t pm_total_pages = 0;

uint64_t _hhdm_offset;

int pmm_init(void *memmap, uint64_t hhdm_offset)
{
    if (memmap == NULL)
    {
        UNUSED(hhdm_offset);
        ERROR("Memory map is null, ensure that the configured bootloader provides a valid memory map.");
        return 1;
    }

    if (hhdm_offset == 0)
    {
        ERROR("HHDM offset is zero, ensure that the configured bootloader provides a valid HHDM offset.");
        return 1;
    }

    DEBUG("HHDM Offset: 0x%llx", hhdm_offset);

    struct limine_memmap_response *_memmap = (struct limine_memmap_response *)memmap;
    _hhdm_offset = hhdm_offset;

    uint64_t pm_stack_size = 0;

    for (uint64_t i = 0; i < _memmap->entry_count; i++)
    {
        struct limine_memmap_entry *entry = _memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            pm_total_pages += entry->length / PAGE_SIZE;
        }
    }

    pm_stack_size = ALIGN_UP((pm_total_pages * sizeof(uint64_t)), PAGE_SIZE);
    DEBUG("PM Stack size: %llu bytes", pm_stack_size);

    // Locate memory for the stack
    for (int64_t i = _memmap->entry_count - 1; i >= 0; i--)
    {
        struct limine_memmap_entry *entry = _memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE && entry->length >= pm_stack_size)
        {
            pm_stack = (uint64_t *)(entry->base + entry->length - pm_stack_size + hhdm_offset);
            memset(pm_stack, 0, pm_stack_size);
            entry->length -= pm_stack_size;
            DEBUG("PM Stack allocated at 0x%.16llx, updated entry base to 0x%.16llx, length to %llu",
                  (uint64_t)pm_stack, entry->base, entry->length);
            break;
        }
    }

    // Push all usable pages to the stack
    for (uint64_t i = 0; i < _memmap->entry_count; i++)
    {
        struct limine_memmap_entry *entry = _memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE)
        {
            for (uint64_t j = 0; j < entry->length; j += PAGE_SIZE)
            {
                if (pm_stack_idx >= pm_total_pages)
                {
                    ERROR("PM Stack overflow while pushing pages.");
                    return 1;
                }
                pm_stack[pm_stack_idx++] = entry->base + j;
            }
        }
    }

    pm_free_pages = pm_stack_idx;
    DEBUG("PM initialized with %llu free pages", pm_free_pages);
    return 0;
}

void *pmm_request_pages(size_t pages)
{
    if (pages == 0 || pm_stack_idx < pages)
    {
        WARN("Unable to allocate %zu pages, out of memory.", pages);
        return NULL;
    }

    pm_stack_idx -= pages;
    void *ptr = (void *)(pm_stack[pm_stack_idx] + _hhdm_offset);
    pm_used_pages += pages;
    pm_free_pages -= pages;

    TRACE("Allocated %zu pages starting at address 0x%llx", pages, (uint64_t)ptr);
    return ptr;
}

void pmm_free_pages(void *ptr, size_t pages)
{
    ASSERT(ptr != NULL);
    ASSERT((uint64_t)ptr % PAGE_SIZE == 0);
    ASSERT(pages > 0);

    uint64_t page_addr = (uint64_t)ptr - _hhdm_offset;

    for (size_t i = 0; i < pages; i++)
    {
        if (pm_stack_idx >= pm_total_pages)
        {
            ERROR("PMM Stack overflow while freeing pages.");
            return;
        }
        pm_stack[pm_stack_idx++] = page_addr + (i * PAGE_SIZE);
    }

    pm_free_pages += pages;
    pm_used_pages -= pages;

    TRACE("Freed %zu pages starting from address 0x%llx", pages, (uint64_t)ptr);
}
