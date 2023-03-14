#include <alloc.h>
#include <panic.h>
#include <printk.h>
#include <string.h>

struct buddy_free_area buddy_free_area[MAX_ORDER + 1];  //!< Buddy freelist.
// TODO:
#define SIZE PAGE_SIZE * 1024
__attribute__((
    aligned(4096))) char buddy_pool[SIZE];  //!< Buddy allocator pool.

void buddy_init(void) {
    for (int i = 0; i <= MAX_ORDER; i++) {
        buddy_free_area[i].freelist = NULL;
        buddy_free_area[i].nr_free = 0;
    }
    // Split pool by 2**MAX_ORDER and link all
    size_t size = (u64)&buddy_pool[(SIZE)] - (u64)&buddy_pool[0];
    char *p = buddy_pool;
    INFO_PRINTK("buddy_init: size:%x\n", size);
    for (int order = MAX_ORDER; order; order--) {
        size_t s = size;
        for (int i = 0; i < (s / (BSIZE << order)); i++) {
            VERBOSE_PRINTK("buddy_init: order: %x, %x\n", order, i);
            ((struct buddy_header *)p)->next = buddy_free_area[order].freelist;
            buddy_free_area[order].freelist = (struct buddy_header *)p;
            buddy_free_area[order].nr_free++;
            p += (BSIZE << order);
            size -= (BSIZE << order);
        }
    }
}

void *buddy_alloc(u8 order) {
    void *p = NULL;

    DEBUG_PRINTK("buddy_alloc: [%x]\n", order);
    if (order > MAX_ORDER) {
        return NULL;
    }

    if (buddy_free_area[order].freelist != NULL) {
        p = buddy_free_area[order].freelist;
        buddy_free_area[order].freelist = buddy_free_area[order].freelist->next;
        if (buddy_free_area[order].nr_free == 0) {
            panic("NO free area\n");
        }
        buddy_free_area[order].nr_free--;
        return p;
    }
    if (buddy_free_area[order].freelist == NULL) {
        p = buddy_alloc(order + 1);
        if (p == NULL) {
            return NULL;
        }
        buddy_free_area[order].freelist = p + (BSIZE << order);
        buddy_free_area[order].freelist->next = NULL;
        buddy_free_area[order].nr_free = 2;
        buddy_free_area[order].nr_free--;
        return p;
    }
    panic("buddy_alloc: Not reach here\n");
    return p;
}

void buddy_free(void *p, u8 order) {
    void *x = NULL;
    DEBUG_PRINTK("buddy_free: [%x] %x\n", order, p);

    if (order > MAX_ORDER) {
        return;
    }

    for (struct buddy_header *b = buddy_free_area[order].freelist; b;
         b = b->next) {
        if ((((u64)p + (BSIZE << order)) == (u64)b) ||
            (((u64)p - (BSIZE << order)) == (u64)b)) {
            DEBUG_PRINTK("Found buddy: [%x] %x, %x\n", order, p, b);
            buddy_free_area[order].freelist =
                buddy_free_area[order].freelist->next;
            buddy_free_area[order].nr_free--;
            x = ((u64)p > (u64)b) ? b : p;
            buddy_free(x, order + 1);
            return;
        }
    }

    if (buddy_free_area[order].freelist == NULL) {
        DEBUG_PRINTK("return to emtpy freelist: [%x] %x\n", order, p);
        buddy_free_area[order].nr_free++;
        buddy_free_area[order].freelist = p;
        buddy_free_area[order].freelist->next = NULL;
    } else {
        DEBUG_PRINTK("return to freelist: [%x] %x\n", order, p);
        buddy_free_area[order].nr_free++;
        ((struct buddy_header *)p)->next = buddy_free_area[order].freelist;
        buddy_free_area[order].freelist = p;
    }
}
