#include <syscall.h>

.text
.globl _dup
_dup:
	li a7, __NR_DUP
	ecall
	ret
