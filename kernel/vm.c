#include <alloc.h>
#include <lib.h>
#include <os1.h>
#include <plic.h>
#include <printk.h>
#include <proc.h>
#include <riscv.h>
#include <sys/types.h>
#include <uart.h>
#include <virtio.h>
#include <vm.h>

void free_pages(void *p, int order) {
    buddy_free(p, order);
}

void *alloc_pages(int order) {
    return buddy_alloc(order);
}

void kmeminit(void) {
    buddy_init(&_end, PHYEND);
}

pte_t *kvmalloc(pagetable_t pgtbl, u64 va) {
    pte_t *pte;

    for (int level = 2; level > 0; level--) {
        pte = &pgtbl[VA2IDX(va, level)];

        if (*pte & PTE_V) {
            pgtbl = (pagetable_t)PTE2PA(*pte);
        } else {
            pgtbl = (pagetable_t)alloc_page();
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
        } else {
            return NULL;
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

void kvmunmap(pagetable_t pgtbl, u64 va, u64 sz) {
    pte_t *pte;

    for (u64 i = ROUNDDOWN(va); i < ROUNDDOWN(va + sz); i += PAGE_SIZE) {
        pte = kvmwalk(pgtbl, i);

        if (pte == NULL) {
            continue;
        }
        if (*pte == 0) {
            continue;
        }

        u64 pa = PTE2PA(*pte);
        free_page((void *)pa);
        *pte = 0;
    }
}

void uvmmap(pagetable_t pgtbl, u64 va, u64 pa, u64 sz, u64 perm) {
    kvmmap(pgtbl, va, pa, sz, perm | PTE_U);
}

void uvmunmap(pagetable_t pgtbl, u64 va, u64 sz) {
    kvmunmap(pgtbl, va, sz);
}

int uvmcopy(pagetable_t dst, pagetable_t src, u64 sz) {
    pte_t *pte;
    u8 *pa;

    for (u64 i = 0; i < sz; i += PAGE_SIZE) {
        pte = kvmwalk(src, i);
        if (pte == NULL || *pte == 0) {
            continue;
        }
        pa = alloc_page();
        memmove((void *)pa, (void *)PTE2PA(*pte), PAGE_SIZE);
        uvmmap(dst, i, (u64)pa, PAGE_SIZE, PTE_FLAGS(*pte));
    }
    return 0;
}

u64 va2pa(pagetable_t pgtbl, u64 va) {
    pte_t *pte;
    u64 pa = 0;

    pte = kvmwalk(pgtbl, va);
    if (pte != NULL) {
        pa = (PTE2PA(*pte) | (va & ((1 << PAGE_OFFSET) - 1)));
    }

    return pa;
}

void kvmdump(pagetable_t pgtbl, u64 va) {
    pte_t *pte;

    pte = kvmwalk(pgtbl, va);
    if (pte == NULL || *pte == 0) {
        VERBOSE_PRINTK("%x: not mapped\n", va);
        return;
    }
    VERBOSE_PRINTK("%x: %p, attrs:%x\n", va, PTE2PA(*pte), PTE_FLAGS(*pte));
}

pagetable_t kvminit(void) {
    pagetable_t kpgtbl;

    kmeminit();
    kpgtbl = alloc_page();
    memset(kpgtbl, 0, PAGE_SIZE);
    kvmmap(kpgtbl, (u64)TRAMPOLINE, (u64)trampoline, PAGE_SIZE,
           PTE_R | PTE_X | PTE_V);
    kvmmap(kpgtbl, 0x80000000, 0x80000000, (u64)&_etext - 0x80000000,
           PTE_R | PTE_X | PTE_V | PTE_W);
    kvmmap(kpgtbl, PLIC_BASE_ADDR, PLIC_BASE_ADDR, 0x4000000,
           PTE_R | PTE_V | PTE_W);
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

int copyin(const void *uaddr, void *kaddr, size_t len) {
    u64 pa = va2pa(this_proc()->pgtbl, (u64)uaddr);

    if (pa == 0) {
        return -1;
    }
    memmove(kaddr, (void *)pa, len);

    return 0;
}

int copyout(const void *kaddr, void *uaddr, size_t len) {
    u64 pa = va2pa(this_proc()->pgtbl, (u64)uaddr);

    if (pa == 0) {
        return -1;
    }
    memmove((void *)pa, kaddr, len);

    return 0;
}

int copyinstr(const void *uaddr, void *kaddr, size_t len, size_t *done) {
    u64 pa = va2pa(this_proc()->pgtbl, (u64)uaddr);

    for (int i = 0; i < len; i++) {
        if (((char *)pa)[i] == '\0') {
            *done = i + 1;
            break;
        }
        ((char *)kaddr)[i] = ((char *)pa)[i];
    }

    return 0;
}
