#include <alloc.h>
#include <printk.h>
#include <slob.h>
#include <stddef.h>

static struct slob_header arena = {.next = &arena, .size = 0};
static struct slob_header *freelist = &arena;

void *kmalloc(size_t size) {
    return NULL;
}

/**
 * Memory area is maintained in freelist in increasing order of memory address.
 * Search insertion point from start.
 * Insertion points is either one of begining, in-between, or ending.
 * If freed block is adjacent of blocks in freelist, merge them.
 */
void kfree(void *p) {
    struct slob_header *b;
    struct slob_header *cur;

    b = (struct slob_header *)p;
    if (p == NULL) {
        WARN_PRINTK("kfree: try to free NULL\n");
        return;
    }

    for (cur = freelist; !(cur < b && b < cur->next); cur = cur->next) {
        /* block to be freed is not in between the block in freelist.*/
        if (cur->next <= cur && (b < cur || b < cur->next)) {
            // freed block is either at start or end.
            break;
        }
    }

    // Insertion point: block to be freed is younger than next block.
    if ((b + b->size) == (cur->next)) {
        // Merge block to be freed(b) and cur->next.
        b->size += cur->next->size;
        b->next = cur->next->next;
    } else {
        b->next = cur->next;
    }

    // Insertion point: block to be freed is older than next block.
    if ((cur + (u64)cur->next) == b) {
        cur->size += b->size;
        cur->next = b->next;
    } else {
        cur->next = b;
    }
    freelist = cur;
}
