#include <os1.h>
#include <riscv.h>
#include <sys/types.h>
#include <timer.h>

struct mscratch mscratch;

void init_timer(void) {
    u64 interval = 1000000;

    *((u64 *)CLINT_MTIMECMP) = *((u64 *)CLINT_MTIME) + interval;
    mscratch.mtimecmp = (u64 *)CLINT_MTIMECMP;
    mscratch.interval = interval;

    w_mscratch((u64)((u64 *)&mscratch));
    w_mtvec((u64)((u64 *)&timervec));
    w_mstatus(r_mstatus() | MSTATUS_MIE);
    w_mie(r_mie() | MIE_MTIE);
}
