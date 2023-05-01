#include <syscall.h>

.text
.globl _fstat
_fstat:
	li a7, __NR_FSTAT
	ecall
	ret
