#include <lib.h>

int strncmp(const char *s1, const char *s2, size_t n) {
    while (*s1 || n) {
        if (*s1 != *s2) {
            break;
        }
        n--;
        s1++;
        s2++;
    }
    return *((const unsigned char *)s1) - *((const unsigned char *)s2);
}
