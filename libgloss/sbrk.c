#include <errno.h>
#include <stdint.h>

#undef errno
extern int errno;

#include <errno.h>
#include <sys/types.h>

static char *heap_start = NULL;
extern char _heap_start;

void *sbrk(ptrdiff_t incr) {
    char *prev_heap_end;

    if (heap_start == NULL) {
        heap_start = (char *)&_heap_start;
    }

    prev_heap_end = heap_start;
    heap_start += incr;

    return (void *)prev_heap_end;
}
