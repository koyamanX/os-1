#include <syscall.h>

.text
.globl fstat
fstat:
	li a7, __NR_FSTAT
	ecall
	ret
