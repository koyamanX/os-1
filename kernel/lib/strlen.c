#include <lib.h>

size_t strlen(const char *s) {
    const char *p = s;
    size_t cnt = 0;

    while (*p != '\0') {
        p++;
        cnt++;
    }

    return cnt;
}
