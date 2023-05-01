#include <syscall.h>

.text
.globl _sbrk
_sbrk:
	li a7, __NR_SBRK
	ecall
	ret
