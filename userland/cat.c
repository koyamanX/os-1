#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    int fd;
    int n;
    char buf[256];

    fd = open("/hello.txt", O_RDONLY, 0);

    while ((n = read(fd, buf, 256)) != 0) {
        write(STDOUT_FILENO, buf, n);
    }

    return 0;
}
