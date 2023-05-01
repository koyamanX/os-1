#ifndef VIRTIO_H
#define VIRTIO_H

#include <riscv.h>

enum {
    MAGIC_VALUE = 0x000,
    VERSION = 0x004,
    DEVICE_ID = 0x008,
    VENDOR_ID = 0x00c,
    DEVICE_FEATURES = 0x010,
    DEVICE_FEATURES_SEL = 0x014,
    DRIVER_FEATURE = 0x020,
    DRIVER_FEATURE_SEL = 0x024,
    QUEUE_SEL = 0x030,
    QUEUE_NUM_MAX = 0x034,
    QUEUE_NUM = 0x038,
    QUEUE_READY = 0x044,
    QUEUE_NOTIFY = 0x050,
    INTERRUPT_STATUS = 0x060,
    INTERRUPT_ACK = 0x064,
    STATUS = 0x070,
    QUEUE_DESC_LOW = 0x080,
    QUEUE_DESC_HIGH = 0x084,
    QUEUE_DRIVER_LOW = 0x090,
    QUEUE_DRIVER_HIGH = 0x094,
    QUEUE_DEVICE_LOW = 0x0a0,
    QUEUE_DEVICE_HIGH = 0x0a4,
    CONFIG_GENERATION = 0x0fc,
    CONFIG = 0x100,
};

#define VIRTIO_BASE 0x10001000
#define VIRTIO_OFFSET(offset) ((volatile u32 *)((VIRTIO_BASE) + (offset)))

#define VIRTIO_MAGIC_VALUE 0x74726976
#define VIRTIO_BLOCK_DEVICE 0x2
#define VIRTIO_VERSION 0x2

#define VIRTIO_STATUS_ACKNOWLEDGE 1
#define VIRTIO_STATUS_DRIVER 2
#define VIRTIO_STATUS_FAILED 128
#define VIRTIO_STATUS_FEATURES_OK 8
#define VIRTIO_STATUS_DRIVER_OK 4
#define VIRTIO_STATUS_DEVICE_NEEDS_RESET 64

#define VIRTIO_BLK_F_RO 5
#define QUEUE_SIZE 8

struct virtq_desc {
    u64 addr;
    u32 len;
#define VIRTQ_DESC_F_NEXT 1
#define VIRTQ_DESC_F_WRITE 2
#define VIRTQ_DESC_F_INDIRECT 4
    u16 flags;
    u16 next;
};

struct virtq_avail {
#define VIRTQ_AVAIL_F_NO_INTERRUPT 1
    u16 flags;
    u16 idx;
    u16 ring[QUEUE_SIZE];
    u16 used_event;
};

struct virtq_used_elem {
    u32 id;
    u32 len;
};

struct virtq_used {
#define VIRTQ_USED_F_NO_NOTIFY 1
    u16 flags;
    u16 idx;
    struct virtq_used_elem ring[QUEUE_SIZE];
    u16 avail_event;
};

struct virtio_blk_req {
#define VIRTIO_BKL_T_IN 0
#define VIRTIO_BKL_T_OUT 1
#define VIRTIO_BLK_T_FLUSH 4
#define VIRTIO_BLK_T_DISCARD 11
#define VIRTIO_BKL_T_WRITE_ZEROS 13
    u32 type;
    u32 reserved;
    u64 sector;
};

typedef u8 virtio_blk_req_data;
typedef u8 virtio_blk_req_status;

typedef struct {
    struct virtq_desc *desc;
    struct virtq_avail *avail;
    struct virtq_used *used;
    struct virtio_blk_req request[QUEUE_SIZE];
    virtio_blk_req_status status[QUEUE_SIZE];
} block_device_t;

extern block_device_t block_device;
void virtio_init(void);
int virtio_req(char *buf, u64 blkno, u8 write);

#define BLOCKSIZE 1024
#define SECTORSIZE 512

#endif
