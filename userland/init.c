#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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

    pid = fork();
    if (pid == 0) {
        write(STDOUT_FILENO, "child\n", 6);
        exit(0);
    } else {
        write(STDOUT_FILENO, "parent\n", 7);
    }

    while (1) {
        asm volatile("nop");
    }

    return 0;
}
