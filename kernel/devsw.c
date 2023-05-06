#include <devsw.h>
#include <stddef.h>
#include <uart.h>
#include <virtio.h>

struct bdevsw bdevsw[NBDEVSW] = {
    {NULL, NULL, virtio_req, NULL},  //!< 0: virtio block device
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
};

struct cdevsw cdevsw[NCDEVSW] = {
    {NULL, NULL, uart_getc, uart_putchar},  //!< 0: console
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
    {NULL, NULL, NULL, NULL},
};

struct mount mount[NMOUNT];  //!< Mount table
dev_t rootdev = 0;           //!< Root device number
