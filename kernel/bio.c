#include <buf.h>
#include <devsw.h>
#include <virtio.h>
#include <vm.h>

static struct buf buf[NBUF];
static struct buf *bfreelist;

void binit(void) {
    // Initialize all buffer entries
    for (int i = 0; i < NBUF; i++) {
        buf[i].valid = 0;
        buf[i].dirty = 0;
        buf[i].next = (i == NBUF - 1) ? NULL : &buf[i + 1];
    }

    // Set bfreelist to the first buffer entry
    bfreelist = &buf[0];
}

struct buf *bread(dev_t dev, u64 blkno) {
    struct buf *bp = getblk(dev, blkno);
    // Read the block into the buffer from the device
    (*bdevsw[dev.major].strategy)(bp->data, bp->blkno, 0);
    bp->valid = 1;

    return bp;
}

void bwrite(struct buf *bp) {
    if (bp == NULL) {
        return;
    }

    if (!bp->dirty || !bp->valid) {
        // Buffer is not dirty or not valid, no need to write
        return;
    }

    struct bdevsw *dev = &bdevsw[bp->dev.major];

    // Call device specific write function
    dev->strategy(bp->data, bp->blkno, 1);

    // Mark buffer as clean
    bp->dirty = 0;
}

void bflush(dev_t dev) {
    struct buf *bp;
    struct bdevsw *bdev = &bdevsw[dev.major];

    // Flush all bpfers associated with dev from the active list
    for (bp = bdev->bactivelist; bp; bp = bp->next) {
        if (bp->dev.major == dev.major && bp->dev.minor == dev.minor &&
            bp->dirty && bp->valid) {
            bwrite(bp);
        }
    }
}

/**
 * @brief Get a buffer from the buffer cache for a given device and block
 * number.
 * @details If a buffer with the given device and block number already exists in
 * the buffer cache, return it. Otherwise, return a new buffer from the buffer
 * cache that can be associated with the device and block number.
 * @param dev Device identifier.
 * @param blkno Block number.
 * @return Pointer to the buffer for the given device and block number, or NULL
 * if no buffers are available.
 */
struct buf *getblk(dev_t dev, u64 blkno) {
    struct buf *bp;

    // Search for a free bpfer in the bpfer cache
    for (bp = bfreelist; bp != NULL; bp = bp->next) {
        if (bp->valid == 0) {
            // Found a free bpfer, remove it from the free list
            if (bp == bfreelist) {
                bfreelist = bp->next;
            } else {
                // Find the previous bpfer in the list and link it to the next
                // bpfer
                struct buf *prev = bfreelist;
                while (prev->next != bp) {
                    prev = prev->next;
                }
                prev->next = bp->next;
            }
            break;
        }
    }

    // If no free bpfer was found, pick one from the active list and flush its
    // contents
    if (bp == NULL) {
        struct bdevsw *bdev = &bdevsw[dev.major];
        bp = bdev->bactivelist;
        if (bp == NULL) {
            // No bpfers available, return NULL
            return NULL;
        }
        bwrite(bp);
    }

    // Set bpfer fields to new values
    bp->valid = 1;
    bp->dev = dev;
    bp->blkno = blkno;
    bp->dirty = 0;

    // Return the bpfer
    return bp;
}

/**
 * Release a buffer back to the buffer cache.
 *
 * @param bp The buffer to release.
 */
void brelse(struct buf *bp) {
    if (!bp) {
        // Do nothing if the buffer pointer is null.
        return;
    }

    if (bp->dirty && bp->valid) {
        // If the buffer is dirty and valid, write it out and mark it as not
        // dirty.
        bwrite(bp);     // Write the contents of the buffer to disk.
        bp->dirty = 0;  // Clear the dirty flag to indicate that the buffer is
                        // no longer dirty.
    }

    bp->valid = 0;
    bp->next = bfreelist;  // Set the next pointer of the buffer to the current
                           // head of the freelist.
    bfreelist = bp;  // Update the head of the freelist to point to the buffer.
}
