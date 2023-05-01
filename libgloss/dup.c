#include <unistd.h>

extern int _dup(int oldfd);

int dup(int oldfd) {
    return _dup(oldfd);
}
