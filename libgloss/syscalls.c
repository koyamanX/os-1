#include <machine/syscall.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

char *__env[1] = {0};
char **environ = __env;

#include <errno.h>
#undef errno
extern int errno;

int _access(const char *pathname, int mode) {
    int ret = -1;
    errno = ENOENT;

    return ret;
}
int _faccessat(int dirfd, const char *pathname, int mode, int flags) {
    int ret = -1;
    errno = ENOENT;

    return ret;
}
int _fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) {
    int ret = -1;
    errno = ENOENT;

    return ret;
}
off_t _lseek(int fd, off_t offset, int whence) {
    int ret = -1;

    if ((fd == STDOUT_FILENO) || (fd == STDERR_FILENO)) {
        ret = 0;
    } else {
        errno = EBADF;
    }

    return (off_t)ret;
}
int _lstat(const char *pathname, struct stat *statbuf) {
    int ret = -1;
    errno = EACCES;

    return ret;
}
int _openat(int dirfd, const char *pathname, int flag, mode_t mode) {
    int ret = -1;
    errno = ENOSYS;

    return ret;
}
int _unlink(const char *pathname) {
    int ret = -1;
    errno = ENOENT;

    return ret;
}
int _gettimeofday(struct timeval *tv, struct timezone *tz) {
    int ret = -1;
    errno = EFAULT;

    return ret;
}
int _isatty(int fd) {
    return -1;
}
int _kill(pid_t pid, int sig) {
    return -1;
}
pid_t _getpid(void) {
    return -1;
}
