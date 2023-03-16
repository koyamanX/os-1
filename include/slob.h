/**
 * @file slob.h
 * @brief Slob allocator
 * @author ckoyama(koyamanX)
 */
#ifndef _SLOB_H
#define _SLOB_H

#include <sys/types.h>

/**
 * @brief Allocate size bytes from kernel heap.
 * @details Allocate size bytes from kernel heap, if size is larger than
 * PAGE_SIZe, it allocate PAGE_SIZE aligned size by directly calling
 * alloc_pages.
 * @param[in] size size to allocate.
 * @return pointer to allocated area.
 */
void *kmalloc(size_t size);

/**
 * @brief Free memory area allocated by kmalloc.
 * @detail Free memory area allocated by kmalloc.
 * @param[in] p pointer to memory area to free.
 */
void kfree(void *p);

/**
 * @brief kmalloc data structure.
 * @details header is maintained in following format.
 * There is no data field, area followed by slob_header is used as data area.
 * | slob_header | data |
 * Actual size of data allocated is sizeof(struct slob_header) + ALIGN(data).
 */
struct slob_header {
    u64 size;
    struct slob_header *next;
};

struct big_slob_header {
    int order;
    void *page;
    struct big_slob_header *next;
};

#endif  //_SLOB_H
