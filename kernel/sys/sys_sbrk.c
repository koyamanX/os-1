#include <printk.h>
#include <proc.h>
#include <vm.h>

void *sbrk(ptrdiff_t increment) {
    struct proc *rp;
    void *addr;

    rp = this_proc();
    addr = (void *)ROUNDDOWN((void *)rp->heap);
    for (u64 i = 0; i < increment; i += PAGE_SIZE) {
        pte_t *pte;

        pte = kvmwalk(rp->pgtbl, (u64)(addr + i));
        if (pte == NULL || *pte == 0) {
            kvmmap(rp->pgtbl, (u64)(addr + i), (u64)alloc_page(), PAGE_SIZE,
                   PTE_V | PTE_W | PTE_R | PTE_X | PTE_U);
        }
        kvmdump(rp->pgtbl, (u64)(addr + i));
    }
    rp->heap = rp->heap + increment;

    return addr;
}
