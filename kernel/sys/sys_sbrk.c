#include <proc.h>
#include <vm.h>

void *sbrk(intptr_t increment) {
    struct proc *rp;
    void *addr;

    rp = this_proc();
    addr = (void *)rp->heap;
    for (u64 i = 0; i < ROUNDUP(increment); i -= PAGE_SIZE) {
        kvmmap(rp->pgtbl, rp->heap + i, (u64)alloc_page(), PAGE_SIZE,
               PTE_V | PTE_W | PTE_R | PTE_X | PTE_U);
        kvmdump(rp->pgtbl, rp->heap + i);
    }
    rp->heap = ROUNDUP(increment);

    return addr;
}
