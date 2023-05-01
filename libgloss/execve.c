#include <unistd.h>

extern int execv(const char *pathname, char *const argv[]);

int _execve(const char *pathname, char *const argv[], char *const envp[]) {
    return execv(pathname, argv);
}
