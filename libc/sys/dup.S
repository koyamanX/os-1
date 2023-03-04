#include <syscall.h>

.text
.globl dup
dup:
	li a7, __NR_DUP
	ecall
	ret
