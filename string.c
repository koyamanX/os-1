#include "string.h"
#include "riscv.h"

char *strcpy(char *dest, const char *src) {
	char *p = dest;

	while(*src) {
		*dest++ = *src++;
	}
	*dest = '\0';
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

int strcmp(const char *s1, const char *s2) {
	return strncmp(s1, s2, strlen((char *)s1));
}
int strncmp(const char *s1, const char *s2, size_t n) {
	while(*s1 || n) {
		if(*s1 != *s2) {
			break;
		}
		n--;
		s1++;
		s2++;
	}
	return *((const unsigned char *) s1) - *((const unsigned char *) s2);
}

char *strtok(char *str, const char *delim) {
	static char *saveptr = NULL;
	char *bp;
	char *ret;

	if(str) {
		saveptr = str;
	}
	if(!saveptr) {
		return NULL;
	}

	bp = saveptr;
	for(const char *c = delim; *c; c++) {
		if(*bp == *c) {
			bp++;
			saveptr = bp;
			break;
		}
	}

	while(*bp) {
		for(const char *c = delim; *c; c++) {
			if(*bp == *c) {
				*bp = '\0';
				ret = saveptr;
				saveptr = bp + 1;
				return ret;
			}
		}
		bp++;
	}
	ret = saveptr;
	saveptr = NULL;
	return ret;
}
