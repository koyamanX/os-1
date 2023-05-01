#include <syscall.h>

.text
.globl _stat
_stat:
	li a7, __NR_STAT
	ecall
	ret
