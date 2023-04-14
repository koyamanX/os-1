#ifndef _STDIO_H
#define _STDIO_H

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

typedef struct _iobuf {
    int cnt;
    char *ptr;
    char *base;
    int flag;
    int fd;
} FILE;

#define OPEN_MAX 20
#define EOF -1
#define NULL ((void *)0)
#define BUFSIZ 4096
extern FILE _iob[OPEN_MAX];

#define stdin &_iob[0]
#define stdout &_iob[1]
#define stderr &_iob[2]

enum _flags { _READ = 01, _WRITE = 02, _UNBUF = 04, _EOF = 010, _ERR = 020 };

int fprintf(FILE *stream, const char *format, ...);

#endif
