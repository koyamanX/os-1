#include <alloc.h>
#include <os1.h>
#include <printk.h>
#include <riscv.h>
#include <string.h>
#include <sys/types.h>
#include <uart.h>
#include <virtio.h>
#include <vm.h>

struct kmem kmem;

void kfree(void *pa) {
    buddy_free(pa, MIN_ORDER);
}

void *kalloc(void) {
    return buddy_alloc(MIN_ORDER);
}

void kmeminit(void) {
    buddy_init();
}

pte_t *kvmalloc(pagetable_t pgtbl, u64 va) {
    pte_t *pte;

    for (int level = 2; level > 0; level--) {
        pte = &pgtbl[VA2IDX(va, level)];

        if (*pte & PTE_V) {
            pgtbl = (pagetable_t)PTE2PA(*pte);
        } else {
            pgtbl = (pagetable_t)kalloc();
            memset(pgtbl, 0, PAGE_SIZE);
            *pte = PA2PTE(pgtbl) | PTE_V;
        }
    }
    return &pgtbl[VA2IDX(va, 0)];
}

pte_t *kvmwalk(pagetable_t pgtbl, u64 va) {
    pte_t *pte;

    for (int level = 2; level > 0; level--) {
        pte = &pgtbl[VA2IDX(va, level)];

        if (*pte & PTE_V) {
            pgtbl = (pagetable_t)PTE2PA(*pte);
        }
    }
    return &pgtbl[VA2IDX(va, 0)];
}

void kvmmap(pagetable_t pgtbl, u64 va, u64 pa, u64 sz, u64 perm) {
    pte_t *pte;

    for (u64 i = ROUNDDOWN(va); i < ROUNDDOWN(va + sz);
         i += PAGE_SIZE, pa += PAGE_SIZE) {
        pte = kvmalloc(pgtbl, i);

        *pte = PA2PTE(pa) | perm;
    }
}

u64 va2pa(pagetable_t pgtbl, u64 va) {
    pte_t *pte;
    u64 pa;

    pte = kvmwalk(pgtbl, va);
    pa = (PTE2PA(*pte) | (va & ((1 << PAGE_OFFSET) - 1)));

    return pa;
}

void kvmdump(pagetable_t pgtbl, u64 va) {
    pte_t *pte;

    pte = kvmwalk(pgtbl, va);
    if (*pte == 0) {
        VERBOSE_PRINTK("%x: not mapped\n", va);
        return;
    }
    VERBOSE_PRINTK("%x: %p, attrs:%x\n", va, PTE2PA(*pte), PTE_FLAGS(*pte));
}

pagetable_t kvminit(void) {
    pagetable_t kpgtbl;

    kmeminit();
    kpgtbl = kalloc();
    memset(kpgtbl, 0, PAGE_SIZE);
    kvmmap(kpgtbl, (u64)TRAMPOLINE, (u64)trampoline, PAGE_SIZE,
           PTE_R | PTE_X | PTE_V);
    kvmmap(kpgtbl, 0x80000000, 0x80000000, (u64)&_etext - 0x80000000,
           PTE_R | PTE_X | PTE_V | PTE_W);
    kvmmap(kpgtbl, (u64)&_etext, (u64)&_etext, (u64)PHYEND - (u64)&_etext,
           PTE_R | PTE_V | PTE_W);
    kvmmap(kpgtbl, UART_BASE, UART_BASE, PAGE_SIZE, PTE_W | PTE_R | PTE_V);
    kvmmap(kpgtbl, VIRTIO_BASE, VIRTIO_BASE, PAGE_SIZE, PTE_W | PTE_R | PTE_V);

    return kpgtbl;
}

void kvmstart(pagetable_t kpgtbl) {
    sfence_vma();
    w_satp(SATP(kpgtbl));
    sfence_vma();
}
