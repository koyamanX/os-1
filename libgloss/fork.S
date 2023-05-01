#include <syscall.h>

.text
.globl _fork
_fork:
	li a7, __NR_FORK
	ecall
	ret
