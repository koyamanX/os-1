#ifndef VM_H
#define VM_H

#include <alloc.h>
#include <riscv.h>

void kvmstart(pagetable_t kpgtbl);
pagetable_t kvminit(void);
void kvmdump(pagetable_t pgtbl, u64 va);
void kvmmap(pagetable_t pgtbl, u64 va, u64 pa, u64 sz, u64 perm);
pte_t *kvmwalk(pagetable_t pgtbl, u64 va);
pte_t *kvmalloc(pagetable_t pgtbl, u64 va);
void kmeminit(void);
void *alloc_pages(int order);
void free_pages(void *p, int order);
u64 va2pa(pagetable_t pgtbl, u64 va);

#define alloc_page() alloc_pages(MIN_ORDER)
#define free_page(p) free_pages(p, MIN_ORDER)

#endif
