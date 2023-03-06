#include <syscall.h>

.text
.globl fork
fork:
	li a7, __NR_FORK
	ecall
	ret
