#ifndef STRING_H
#define STRING_H

#include "types.h"

char *strcpy(char *dest, const char *src);
void *memset(void *s, int c, u64 sz);
size_t strlen(char *s);
void *memcpy(void *dest, const void *src, size_t n);

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strtok(char *str, const char *delim);

#endif
