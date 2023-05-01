#include <proc.h>
#include <vm.h>

int brk(void *addr) {
    this_proc()->heap = ROUNDUP((u64)addr);

    return 0;
}
