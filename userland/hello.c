#include <stdio.h>
#include <unistd.h>

int main(void) {
    write(1, "Hello World\n", 12);

    return 0;
}
