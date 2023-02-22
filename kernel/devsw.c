#include <devsw.h>
#include <stddef.h>
#include <virtio.h>

static int nulldev(void) {
	return 0;
}

struct bdevsw bdevsw[NBDEVSW] = {
	{nulldev, nulldev, virtio_req},
	{NULL, NULL, NULL},
	{NULL, NULL, NULL},
	{NULL, NULL, NULL},
};

struct mount mount[NMOUNT];
dev_t rootdev = {0, 0};
