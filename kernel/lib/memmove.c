#include <lib.h>

void *memmove(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;

    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = n; i != 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return dest;
}