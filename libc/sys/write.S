#include <syscall.h>

.text
.globl write
write:
	li a7, __NR_WRITE
	ecall
	ret
