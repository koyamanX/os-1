#include <syscall.h>

.text
.globl _execv
_execv:
	li a7, __NR_EXECEV
	ecall
	ret
