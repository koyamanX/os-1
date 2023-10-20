#include <string.h>
#include <unistd.h>

int main(void) {
    // Implemetation of simple shell which execute one command with no
    // arguments.
    char input[100];

loop:
    // Write prompt with write system call.
    write(1, "sh> ", 4);

    // Read user input one by one character using read system call until new
    // line.
    read(0, input, 100);

    if (strncmp(input, "contributors", strlen("contributors")) == 0) {
        write(1, "@DHA_DHA_DHA\n", 13);
        return 0;
    }

    // execute command in child process
    if (fork() == 0) {
        // Execute command using exec system call.
        execv(input, NULL);
    } else {
        goto loop;
    }

    return 0;
}
