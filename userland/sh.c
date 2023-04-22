#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    // Implemetation of simple shell which execute one command with no
    // arguments.
    char input[100];

    memset(input, 0, 100);

    // Write prompt with write system call.
    write(1, "sh> ", 4);

    // Read user input one by one character using read system call until new
    // line.
    int i = 0;
    while (1) {
        read(0, &input[i], 1);
        write(1, &input[i], 1);
        if (input[i] == '\r') {
            input[i] = '\0';
            write(1, "\n", 1);
            break;
        }
        i++;
    }

    if (strcmp(input, "reten") == 0) {
        write(1, "@DHA_DHA_DHA\n", 13);
        return 0;
    } else if (strcmp(input, "koyaman") == 0) {
        write(1, "@koyamanX\n", 10);
        return 0;
    }

    // Execute command using exec system call.
    exec(input, NULL);

    return 0;
}
