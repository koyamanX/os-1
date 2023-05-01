#include <sys/stat.h>

extern int _mknod(const char *pathname, mode_t mode, dev_t dev);

int mknod(const char *pathname, mode_t mode, dev_t dev) {
    return _mknod(pathname, mode, dev);
}
