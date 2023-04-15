#include <unistd.h>

extern char _heap_start;
static char *heap_start = &_heap_start;
static char *heap_end = NULL;
extern void *_sbrk(intptr_t increment);

void *sbrk(intptr_t increment) {
    // TODO: handle decrement to free heap.
    void *addr = NULL;

    if (heap_end == NULL) {
        heap_end = heap_start;
        if (brk(heap_end) == -1) {
            return NULL;
        }
    }

    if ((heap_start + increment) < heap_end) {
        // Enough room for size.
        addr = heap_start;
        heap_start += increment;
    } else {
        heap_end = _sbrk(increment);
        addr = heap_start;
        heap_start += increment;
    }

    return addr;
}
