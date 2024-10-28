#ifndef OLIVE_VMM_H
#define OLIVE_VMM_H

#include <stdint.h>
#include <stdlib.h>
#include <mm/pmm.h>

typedef struct pagemap
{
    uint64_t pml4;
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

int vmm_init(void);
void vmm_set_pm(pagemap_t *pm);
void vmm_map(pagemap_t *pm, uint64_t virt, uint64_t phys, int flags);
void vmm_unmap(pagemap_t *pm, uint64_t virt);
uint64_t vmm_virt2phys(pagemap_t *pm, uint64_t virt);

#endif // OLIVE_VMM_H