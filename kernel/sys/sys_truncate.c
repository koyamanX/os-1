#include <fs.h>
#include <proc.h>

int truncate(const char *path, off_t length) {
    struct inode *ip;
    size_t size;

    ip = namei(path);
    if (ip == NULL) {
        return -1;
    }
    // TODO: Check write permissions.
    size = ip->size;
    ip->size = length;
    if (size < length) {
        // TODO: zero fill expanded area.
    }
    iupdate(ip);

    return ip->size;
}
