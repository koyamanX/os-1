#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void ls(char *path) {
    int fd;
    struct stat st;

    if ((fd = open(path, O_RDONLY, 0)) < 0) {
        write(1, "err:1\n", 6);
        return;
    }

    if (fstat(fd, &st) < 0) {
        write(1, "err:2\n", 6);
        close(fd);
        return;
    }

    switch (st.st_mode & S_IFMT) {
        case S_IFBLK:
        case S_IFCHR:
        case S_IFREG:
            // printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
            write(1, path, strlen(path));
            write(1, "\n", 1);
            break;
        default:
            write(1, "err:3\n", 6);
            break;
    }
    close(fd);
}

int main(int argc, char *argv[]) {
    ls("/hello.txt");

    return 0;
}
