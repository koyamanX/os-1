#include <alloc.h>
#include <panic.h>
#include <printk.h>
#include <string.h>

struct buddy_header *buddy_freelist[MAX_ORDER + 1];  //!< Buddy freelist.
// TODO:
__attribute__((
    aligned(4096))) char buddy_pool[PAGE_SIZE * 16];  //!< Buddy allocator pool.

void buddy_init(void) {
    for (int i = 0; i < MAX_ORDER; i++) {
        buddy_freelist[i] = NULL;
    }
    buddy_freelist[MAX_ORDER] = (struct buddy_header *)buddy_pool;
    buddy_freelist[MAX_ORDER]->next = NULL;
}

void *buddy_alloc(u8 order) {
    void *p = NULL;

    if (order > MAX_ORDER) {
        return NULL;
    }

    if (buddy_freelist[order] != NULL) {
        p = buddy_freelist[order];
        buddy_freelist[order] = buddy_freelist[order]->next;
        return p;
    }
    if (buddy_freelist[order] == NULL) {
        p = buddy_alloc(order + 1);
        if (p == NULL) {
            return NULL;
        }
        buddy_freelist[order] = p + (BSIZE << order);
        buddy_freelist[order]->next = NULL;
        return p;
    }
    return p;
}

void buddy_free(void *p, u8 order) {
    void *x = NULL;
    DEBUG_PRINTK("buddy_free: [%x] %x\n", order, p);

    if (order > MAX_ORDER) {
        return;
    }

    for (struct buddy_header *b = buddy_freelist[order]; b; b = b->next) {
        if ((((u64)p + (BSIZE << order)) == (u64)b) ||
            (((u64)p - (BSIZE << order)) == (u64)b)) {
            DEBUG_PRINTK("Found buddy: [%x] %x, %x\n", order, p, b);
            buddy_freelist[order] = buddy_freelist[order]->next;
            x = ((u64)p > (u64)b) ? b : p;
            buddy_free(x, order + 1);
            return;
        }
    }

    if (buddy_freelist[order] == NULL) {
        DEBUG_PRINTK("return to emtpy freelist: [%x] %x\n", order, p);
        buddy_freelist[order] = p;
        buddy_freelist[order]->next = NULL;
    } else {
        DEBUG_PRINTK("return to freelist: [%x] %x\n", order, p);
        ((struct buddy_header *)p)->next = buddy_freelist[order];
        buddy_freelist[order] = p;
    }
}
