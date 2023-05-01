#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#undef errno
extern int errno;

pid_t _wait(int *wstatus) {
    errno = ECHILD;

    return -1;
}
