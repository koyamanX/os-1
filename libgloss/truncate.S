#include <syscall.h>

.text
.globl _truncate
_truncate:
	li a7, __NR_TRUNCATE
	ecall
	ret
