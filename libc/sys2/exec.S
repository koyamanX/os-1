#include <syscall.h>

.text
.globl execv
execv:
	li a7, __NR_EXECEV
	ecall
	ret
