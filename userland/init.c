#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    dev_t dev;
    char buf[] = "Hello from init!\n";
    int pid;

    dev.major = 0;
    dev.minor = 0;
    mknod("/console", S_IFCHR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH,
          dev);
    dup(0);
    dup(0);
    int fd = open("/hello.txt", O_RDONLY, S_IRWXU);
    char buffer[1];
    ssize_t bytes;

    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes);
    }
    close(fd);

    write(STDOUT_FILENO, buf, strlen(buf));
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
