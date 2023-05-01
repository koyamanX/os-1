#include <syscall.h>

.text
.globl _read
_read:
	li a7, __NR_READ
	ecall
	ret
