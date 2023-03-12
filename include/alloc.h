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
#define MAX_ORDER 4      //!< Maximum order (2**4)
#define MIN_ORDER 0      //!< Minimum order (2**0)

/**
 * @brief Struct for allocation list.
 * @details Maintain header and its data buffer.
 */
struct buddy_header {
    struct buddy_header *next;  //!< Next free buddy.
};

/**
 * @brief Initialize buddy allocator.
 * @details Initialize buddy allocator before using.
 */
void buddy_init(void);

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
