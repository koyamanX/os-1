#ifndef VM_H
#define VM_H

#include <riscv.h>

void kvmstart(pagetable_t kpgtbl);
pagetable_t kvminit(void);
void kvmdump(pagetable_t pgtbl, u64 va);
void kvmmap(pagetable_t pgtbl, u64 va, u64 pa, u64 sz, u64 perm);
pte_t *kvmwalk(pagetable_t pgtbl, u64 va);
pte_t *kvmalloc(pagetable_t pgtbl, u64 va);
void kmeminit(void);
void *kalloc(void);
void kfreerange(void *pa_start, void *pa_end);
void kfree(void *pa);
u64 va2pa(pagetable_t pgtbl, u64 va);

#endif
