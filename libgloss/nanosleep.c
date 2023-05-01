#include <time.h>

#include <errno.h>
#undef errno
extern int errno;

int nanosleep(const struct timespec *req, struct timespec *rem) {
    int ret = -1;
    errno = EFAULT;

    return ret;
}
