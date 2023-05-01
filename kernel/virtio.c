#include <lib.h>
#include <panic.h>
#include <virtio.h>
#include <vm.h>

block_device_t block_device;

void virtio_init(void) {
    // Init virtio MMIO device for block device
    // We assume block device is at index 0.
    u32 magic_value;
    u32 version;
    u32 device_id;

    magic_value = *VIRTIO_OFFSET(MAGIC_VALUE);
    version = *VIRTIO_OFFSET(VERSION);
    device_id = *VIRTIO_OFFSET(DEVICE_ID);

    if (VIRTIO_MAGIC_VALUE != magic_value) {
        panic("virtio: invalid magic value\n");
    }

    if (version != VIRTIO_VERSION) {
        panic("virtio: unsupported version\n");
    }

    if (device_id == 0) {
        panic("virtio: invalid device id\n");
    }

    if (device_id != VIRTIO_BLOCK_DEVICE) {
        panic("virtio: block device is not found\n");
    }

    /* Device initialization
     * 1. Reset device.
     * 2. Set ACKNOWLEDGE status bit.
     * 3. Set DRIVER status bit.
     * 4. Read feature bits, Write feature bits understood by driver.
     * 5. Set FEATURE_OK status bit.
     * 6. Read status bit to ensure FEATURE_OK is still set.
     * 	if not set, panic.
     * 7. Perform device-specific setup.
     * 8. Set DRIVER_OK status bit.
     */

    // 2. Set ACKNOWLEDGE status bit.
    *VIRTIO_OFFSET(STATUS) = VIRTIO_STATUS_ACKNOWLEDGE;
    // 3. Set DRIVER status bit.
    *VIRTIO_OFFSET(STATUS) = *VIRTIO_OFFSET(STATUS) | VIRTIO_STATUS_DRIVER;
    // 4. Read feature bits, Write feature bits understood by driver.
    u32 features = *VIRTIO_OFFSET(DEVICE_FEATURES);
    // Disable Read-only mode.
    features &= ~(1 << VIRTIO_BLK_F_RO);
    *VIRTIO_OFFSET(DEVICE_FEATURES) = features;
    // 5. Set FEATURE_OK status bit.
    *VIRTIO_OFFSET(STATUS) = *VIRTIO_OFFSET(STATUS) | VIRTIO_STATUS_FEATURES_OK;
    // 6. Read status bit to ensure FEATURE_OK is still set.
    if ((*VIRTIO_OFFSET(STATUS) & VIRTIO_STATUS_FEATURES_OK) == 0) {
        // if not set, panic.
        panic("virtio: unsupported features\n");
    }
    // 7. Perform device-specific setup.
    u32 queue_max = *VIRTIO_OFFSET(QUEUE_NUM_MAX);
    if (QUEUE_SIZE > queue_max) {
        panic("virtio: not enough queue size\n");
    }
    *VIRTIO_OFFSET(QUEUE_SEL) = 0;
    block_device.desc = alloc_page();
    block_device.avail = alloc_page();
    block_device.used = alloc_page();
    memset(block_device.desc, 0, PAGE_SIZE);
    memset(block_device.avail, 0, PAGE_SIZE);
    memset(block_device.used, 0, PAGE_SIZE);

    *VIRTIO_OFFSET(QUEUE_NUM) = QUEUE_SIZE;

    *VIRTIO_OFFSET(QUEUE_DESC_HIGH) = ((u64)(block_device.desc)) >> 32;
    *VIRTIO_OFFSET(QUEUE_DESC_LOW) = ((u64)(block_device.desc));
    *VIRTIO_OFFSET(QUEUE_DRIVER_HIGH) = ((u64)(block_device.avail)) >> 32;
    *VIRTIO_OFFSET(QUEUE_DRIVER_LOW) = ((u64)(block_device.avail));
    *VIRTIO_OFFSET(QUEUE_DEVICE_HIGH) = ((u64)(block_device.used)) >> 32;
    *VIRTIO_OFFSET(QUEUE_DEVICE_LOW) = ((u64)(block_device.used));

    *VIRTIO_OFFSET(QUEUE_READY) = 1;

    // 8. Set DRIVER_OK status bit.
    *VIRTIO_OFFSET(STATUS) = *VIRTIO_OFFSET(STATUS) | VIRTIO_STATUS_DRIVER_OK;
}

int virtio_req(char *buf, u64 blkno, u8 write) {
    struct virtq_desc *desc;
    struct virtq_avail *avail;
    u64 sector;

    // Block size is 1KB, Sector size is 512B,
    // So block number 0 is mapped to sector number 0,1
    // and block number 1 is mapped to sector number 2,3
    // and so on. so multiply blkno by 2 is sector number.
    sector = blkno * 2;
    block_device.request[0].type = (write) ? VIRTIO_BKL_T_OUT : VIRTIO_BKL_T_IN;
    block_device.request[0].sector = sector;
    desc = block_device.desc;
    desc[0].addr = (u64)&block_device.request[0];
    desc[0].len = sizeof(struct virtio_blk_req);
    desc[0].flags = VIRTQ_DESC_F_NEXT;
    desc[0].next = 1;

    desc[1].addr = (u64)buf;
    desc[1].len = BLOCKSIZE;
    desc[1].flags = VIRTQ_DESC_F_NEXT;
    desc[1].flags |= (!write) ? VIRTQ_DESC_F_WRITE : 0;
    desc[1].next = 2;

    block_device.status[0] = 0xff;
    desc[2].addr = (u64)&block_device.status[0];
    desc[2].len = sizeof(virtio_blk_req_status);
    desc[2].flags = VIRTQ_DESC_F_WRITE;
    desc[2].next = 0;

    avail = block_device.avail;
    avail->ring[avail->idx % QUEUE_SIZE] = 0;

    __sync_synchronize();
    avail->idx++;
    __sync_synchronize();

    *VIRTIO_OFFSET(QUEUE_NOTIFY) = 0;

    while (1) {
        if (block_device.status[0] != 0xff) {
            break;
        }
    }

    return 0;
}
