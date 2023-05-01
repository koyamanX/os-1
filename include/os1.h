#ifndef OS1_H
#define OS1_H

#include <riscv.h>

extern char *_end;
extern char *_etext;
extern void trampoline(void);

struct mscratch {
    u64 tmp0;
    u64 tmp1;
    u64 tmp2;
    u64 *mtimecmp;
    u64 interval;
};

void kinit(void);
void kmain(void);
void init_timer(void);
extern void timervec(void);
extern char kernelvec;

#endif
