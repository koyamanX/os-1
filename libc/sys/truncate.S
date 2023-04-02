#include <syscall.h>

.text
.globl truncate
truncate:
	li a7, __NR_TRUNCATE
	ecall
	ret
