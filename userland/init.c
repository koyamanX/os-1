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

    dev.major = 0;
    dev.minor = 0;
    mknod("/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
          dev);
    dup(0);
    dup(0);

    while (1) {
        pid = fork();
        if (pid == 0) {
            execv("/usr/sbin/sh", NULL);
        }

        while (1) {
            int wpid;
            wpid = waitpid(pid, NULL, WNOHANG);
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
