#include <sys/times.h>

#include <errno.h>
#undef errno
extern int errno;

clock_t times(struct tms *buf) {
    int ret = -1;
    errno = EFAULT;

    return (clock_t)ret;
}
