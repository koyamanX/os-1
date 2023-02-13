#ifndef STRING_H
#define STRING_H

#include "types.h"

char *strcpy(char *dest, const char *src);
void *memset(void *s, int c, u64 sz);
size_t strlen(char *s);
void *memcpy(void *dest, const void *src, size_t n);
#endif
