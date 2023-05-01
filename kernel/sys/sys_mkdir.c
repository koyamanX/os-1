#include <fcntl.h>
#include <fs.h>
#include <sys/stat.h>

int mkdir(const char *pathname, mode_t mode) {
    return open(pathname, (O_CREAT), (mode & RWX_MODES) | I_DIRECTORY);
}
