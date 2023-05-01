#include <syscall.h>

.text
.globl _brk
_brk:
	li a7, __NR_BRK
	ecall
	ret
