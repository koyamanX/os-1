#ifndef _LIB_H
#define _LIB_H

#include <libgen.h>
#include <stddef.h>

char *strcpy(char *dest, const char *src);
void *memset(void *s, int c, size_t sz);
size_t strlen(const char *s);
void *memcpy(void *dest, const void *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strtok(char *str, const char *delim);
void *memmove(void *dest, const void *src, size_t n);

#endif /* _LIB_H */
