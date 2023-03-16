#include <string.h>

size_t strlen(char *s) {
    char *p = s;
    size_t cnt = 0;

    while (*p != '\0') {
        p++;
        cnt++;
    }

    return cnt;
}
