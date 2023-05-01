#include <syscall.h>

.text
.globl _open
_open:
	li a7, __NR_OPEN
	ecall
	ret
