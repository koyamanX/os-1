#include <printk.h>
#include <slob.h>
#include <stddef.h>
#include <vm.h>

static struct slob_header arena = {.next = &arena,
                                   .units = 1};  //!< Slob freelist head base.
static struct slob_header *freelist = &arena;    //!< Slob freelist.

void *kmalloc(size_t size) {
    u64 units;
    struct slob_header *cur;
    struct slob_header *prev;

    // size larger than PAGE_SIZE is not supported yet.
    if (size >= PAGE_SIZE) {
        return NULL;
    }

    // Data size(aligned to slob_header) + one slob_header.
    units =
        ((size + sizeof(struct slob_header) - 1) / sizeof(struct slob_header)) +
        1;

    prev = freelist;
    for (cur = prev->next;; prev = cur, cur = cur->next) {
        if (cur->units >= units) {
            // Enough room.
            if (cur->units == units) {
                // Exact fit.
                prev->next = cur->next;
            } else {
                // Split.
                prev->next = cur + units;
                prev->next->units = cur->units - units;
                prev->next->next = cur->next;
                cur->units = units;
            }
            freelist = prev;

			// Skip slob header
            return cur + sizeof(struct slob_header);
        }
        if (cur == freelist) {
            cur = (struct slob_header *)alloc_page();
            kfree(cur);
            cur = freelist;
        }
    }
    // Not reachable.
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
        if (cur->next <= cur && (b > cur || b < cur->next)) {
            // freed block is either at start or end.
            break;
        }
    }

    // Insertion point: block to be freed is younger than next block.
    if ((b + b->units) == (cur->next)) {
        // Merge block to be freed(b) and cur->next.
        b->units += cur->next->units;
        b->next = cur->next->next;
    } else {
        b->next = cur->next;
    }

    // Insertion point: block to be freed is older than next block.
    if ((cur + (u64)cur->next) == b) {
        cur->units += b->units;
        cur->next = b->next;
    } else {
        cur->next = b;
    }
    freelist = cur;
}
