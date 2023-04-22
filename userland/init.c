#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    dev_t dev;
    int pid;
    int wpid;

    dev.major = 0;
    dev.minor = 0;
    mknod("/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
          dev);
    dup(0);
    dup(0);

    while (1) {
        pid = fork();
        if (pid == 0) {
            exec("/usr/sbin/sh", NULL);
        }

        while (1) {
            wpid = waitpid(pid, NULL, 0);
            if (wpid == pid) {
                break;
            }
        }
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    return 0;
}
