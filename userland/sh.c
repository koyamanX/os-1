#include <unistd.h>

int main(void) {
    // Implemetation of simple shell which execute one command with no
    // arguments.
    char input[100];

    // Write prompt with write system call.
    write(1, "sh> ", 4);

    // Read user input one by one character using read system call until new
    // line.
    read(0, input, 100);

#if 0
    if (strncmp(input, "reten", 6) == 0) {
        write(1, "@DHA_DHA_DHA\n", 13);
        return 0;
    } else if (strcmp(input, "koyaman") == 0) {
        write(1, "@koyamanX\n", 10);
        return 0;
    }
#endif

    // Execute command using exec system call.
    char *argv[] = {"/usr/sbin/cat", "/hello.txt", NULL};
    execv(input, argv);

    return 0;
}
