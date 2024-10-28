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