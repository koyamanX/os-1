#include <alloc.h>
#include <lib.h>
#include <panic.h>
#include <printk.h>
#include <vm.h>

struct buddy_free_area buddy_free_area[MAX_ORDER + 1];  //!< Buddy freelist.

void buddy_init(void *start, void *end) {
    size_t size;
    char *p;

    start = (void *)ROUNDUP(start);
    end = (void *)ROUNDDOWN(end);
    size = ((u64)end) - ((u64)start);
    p = (char *)start;

    for (int i = 0; i <= MAX_ORDER; i++) {
        buddy_free_area[i].freelist = NULL;
        buddy_free_area[i].nr_free = 0;
    }
    INFO_PRINTK("buddy_init: size:%x\n", size);
    for (int order = MAX_ORDER; order >= 0; order--) {
        size_t s = size;
        for (int i = 0; i < (s / (BSIZE << order)); i++) {
            buddy_free(p, order);
            p += (BSIZE << order);
            size -= (BSIZE << order);
        }
    }
}

void *buddy_alloc(u8 order) {
    void *p = NULL;

    if (order > MAX_ORDER) {
        panic("buddy_alloc: order > MAX_ORDER");
        return NULL;
    }

    // If there is free block in this order, return it.
    if (buddy_free_area[order].nr_free > 0) {
        p = buddy_free_area[order].freelist;
        // If there is only one block in this order, set freelist to NULL.
        if (buddy_free_area[order].nr_free == 1) {
            buddy_free_area[order].freelist = NULL;
        } else {
            buddy_free_area[order].freelist =
                buddy_free_area[order].freelist->next;
        }
        buddy_free_area[order].nr_free--;

        return p;
    } else {
        p = buddy_alloc(order + 1);
        if (p == NULL) {
            return NULL;
        }
        // Split the block.
        buddy_free(p + (BSIZE << order), order);

        if (p == NULL) {
            panic("buddy_alloc: p == NULL");
            return NULL;
        }
        return p;
    }
}

void buddy_free(void *p, u8 order) {
    void *x = NULL;

    if (order > MAX_ORDER) {
        panic("buddy_free: order > MAX_ORDER");
        return;
    }
    if (p == NULL) {
        panic("buddy_free: p == NULL");
        return;
    }

    if (order < MAX_ORDER) {
        // Prev
        struct buddy_header *prev = NULL;
        for (struct buddy_header *b = buddy_free_area[order].freelist; b;
             prev = b, b = b->next) {
            if ((((u64)p + (BSIZE << order)) == (u64)b) ||
                (((u64)p - (BSIZE << order)) == (u64)b)) {
                if (buddy_free_area[order].nr_free == 1) {
                    buddy_free_area[order].freelist = NULL;
                } else {
                    if (prev == NULL) {
                        buddy_free_area[order].freelist =
                            buddy_free_area[order].freelist->next;
                    } else {
                        prev->next = b->next;
                    }
                }
                buddy_free_area[order].nr_free--;
                x = ((u64)p > (u64)b) ? b : p;
                buddy_free(x, order + 1);
                return;
            }
        }
    }

    if (buddy_free_area[order].nr_free == 0) {
        buddy_free_area[order].freelist = p;
        buddy_free_area[order].freelist->next = NULL;
    } else {
        if (buddy_free_area[order].freelist == NULL) {
            panic("buddy_free: freelist == NULL");
        }
        ((struct buddy_header *)p)->next = buddy_free_area[order].freelist;
        buddy_free_area[order].freelist = p;
    }
    buddy_free_area[order].nr_free++;
}
