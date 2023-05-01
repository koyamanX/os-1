#include <syscall.h>

.text
.globl _write
_write:
	li a7, __NR_WRITE
	ecall
	ret
