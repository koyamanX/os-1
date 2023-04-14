#include <unistd.h>

extern char _heap_start;
static char *heap_start = &_heap_start;
static char *heap_end = NULL;
extern void *_sbrk(intptr_t increment);

void *sbrk(intptr_t increment) {
    if (heap_end == NULL) {
        heap_end = heap_start;
        if (brk(heap_end) == -1) {
            return NULL;
        }
    }

    return _sbrk(increment);
}
