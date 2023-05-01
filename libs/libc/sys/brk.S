#include <syscall.h>

.text
.globl brk
brk:
	li a7, __NR_BRK
	ecall
	ret
