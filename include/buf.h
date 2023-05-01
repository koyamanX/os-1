/**
 * @file buf.h
 * @brief Buffer cache for disk I/O operations.
 * @author ckoyama(koyamanX)
 */

#ifndef _BUF_H
#define _BUF_H

#include <devsw.h>
#include <riscv.h>
#include <sys/types.h>

#define BUFSIZ 1024  //!< Buffer size.
#define NBUF 32      //!< Number of buffer.

/**
 * @brief Struct for buffer cache.
 * @details Buffer cache, it is maintained in linked-list in either active list
 * or freelist.
 */
struct buf {
    int valid;          //!< Is this entry valid?
    dev_t dev;          //!< Block device associated to this entry.
    u64 blkno;          //!< Block number.
    int dirty;          //!< Is this entry dirty?
    char data[BUFSIZ];  //!< Actual content only data is valid on valid is true.
    struct buf *next;   //!< Next entry.
};

/**
 * @brief Initialize buffer cache.
 * @details Initialize buffer cache.
 */
void binit(void);

/**
 * @brief Read block from disk.
 * @details Read block number (blikno) at device (dev), allocate new buffer. If
 * buffer is not available in freelist, free one of entry in device's active
 * list.
 * @param[in] dev device.
 * @param[in] blkno block number to read.
 * @return Pointer to buffer read.
 */
struct buf *bread(dev_t dev, u64 blkno);

/**
 * @brief Write block to disk.
 * @details Write block to device associated to buffer.
 * @param[in] bp pointer to buffer cache to be written.
 * @attention bwrite does not free written buffer.
 */
void bwrite(struct buf *bp);

/**
 * @brief Write active buffer associated to device.
 * @details Write active buffer associated to device.
 * @param[in] dev device.
 */
void bflush(dev_t dev);

/**
 * @brief Get new buffer cache entry.
 * @details Get new buffer cache entry. If none is found in freelist, then flush
 * one of entry in active list assocaiated to device, and return freed entry.
 * @param[in] dev device to assocaiate buffer cache with.
 * @param[in] blkno block number to assocaiate buffer cache with.
 * @return Pointer to buffer.
 */
struct buf *getblk(dev_t dev, u64 blkno);

/**
 * @brief Release buffer cache, flush if dirty.
 * @details Release buffer cache from device active list, if it is dirty, flush
 * to device, then return to freelist.
 */
void brelse(struct buf *bp);

#endif  // _BUF_H
