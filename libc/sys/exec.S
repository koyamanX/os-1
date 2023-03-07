#include <syscall.h>

.text
.globl exec
exec:
	li a7, __NR_EXECEV
	ecall
	ret
