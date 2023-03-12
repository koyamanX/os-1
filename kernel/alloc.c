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

void buddy_free(void *p, u8 order) {}
