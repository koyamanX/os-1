/**
 * @file alloc.h
 * @brief Buddy allocator
 * @author ckoyama(koyamanX)
 */
#ifndef _ALLOC_H
#define _ALLOC_H

#include <riscv.h>
#include <sys/types.h>

#define BSIZE PAGE_SIZE  //!< BLOCK SIZE(minimum allocation size)
#define MAX_ORDER 10     //!< Maximum order (2**10)
#define MIN_ORDER 0      //!< Minimum order (2**0)

/**
 * @brief Struct for allocation list.
 * @details Maintain header and its data buffer.
 */
struct buddy_header {
    struct buddy_header *next;  //!< Next free buddy.
};

/**
 * @brief Struct for free area.
 * @details Maintain free area and its number of free blocks.
 */
struct buddy_free_area {
    struct buddy_header *freelist;
    u64 nr_free;
};

/**
 * @brief Initialize buddy allocator.
 * @details Initialize buddy allocator before using.
 * @param[in] start Start of address range.
 * @param[in] end End of address range.
 * @attention start is internally ROUNDUP to PAGE_SIZE and end
 * is ROUNDDOWN to PAGE_SIZE alignments.
 */
void buddy_init(void *start, void *end);

/**
 * @brief Allocate memory region.
 * @details Allocate appropriate size region.
 * @param[in] order order of memory size. actual size is 2**order.
 * @return Rllocated memory location on success or NULL on failure.
 */
void *buddy_alloc(u8 order);

/**
 * @brief Deallocate memory region.
 * @details Deallocate memory region.
 * @param[in] p pointer to memory location to free
 * @param[in] order order of memory size. actual size is 2**order.
 */
void buddy_free(void *p, u8 order);

#endif  // _ALLOC_H
