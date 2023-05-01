#include <lib.h>

char *strcpy(char *dest, const char *src) {
    char *p = dest;

    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return p;
}
