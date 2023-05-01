#include <sys/types.h>
#include <sys/wait.h>

extern pid_t _waitpid(pid_t pid, int *wstatus, int options);

pid_t waitpid(pid_t pid, int *wstatus, int options) {
    return _waitpid(pid, wstatus, options);
}
