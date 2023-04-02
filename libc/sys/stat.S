#include <syscall.h>

.text
.globl stat
stat:
	li a7, __NR_STAT
	ecall
	ret
