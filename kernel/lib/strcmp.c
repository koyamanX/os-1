#include <lib.h>

int strcmp(const char *s1, const char *s2) {
    return strncmp(s1, s2, strlen((char *)s1));
}
