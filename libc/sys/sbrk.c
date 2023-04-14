#include <unistd.h>

static char *heap = NULL;
extern char _heap_start;
extern void *_sbrk(intptr_t increment);

void *sbrk(intptr_t increment) {
    if (heap == NULL) {
        heap = &_heap_start;
        if (brk(heap) == -1) {
            return NULL;
        }
    }
    return _sbrk(increment);
}
