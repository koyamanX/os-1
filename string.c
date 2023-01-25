#include "string.h"

char *strcpy(char *dest, const char *src) {
	char *p = dest;

	while(*src) {
		*dest++ = *src++;
	}
	return p;
}

void *memset(void *s, int c, u64 sz) {
	char *p = (char *)s;

	for(u64 i = 0; i < sz; i++) {
		p[i] = c;
	}
	return s;
}

