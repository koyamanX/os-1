#include <lib.h>

void *memset(void *s, int c, size_t sz) {
    char *p = (char *)s;

    for (size_t i = 0; i < sz; i++) {
        p[i] = c;
    }
    return p;
}
