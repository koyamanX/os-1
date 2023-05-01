#include <unistd.h>

extern int _execv(const char *pathname, char *const argv[]);

int execv(const char *pathname, char *const argv[]) {
    return _execv(pathname, argv);
}
