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
	return p;
}

void *memcpy(void *dest, const void *src, size_t n) {
	char *p = dest;

	while(n > 0) {
		*((u8 *)dest++)= *((u8 *)src++);
		n--;
	}
	return p;
}

size_t strlen(char *s) {
	char *p = s;
	size_t cnt = 0;

	while(*p != '\0'){
		p++;
		cnt++;
	}

	return cnt;
}
