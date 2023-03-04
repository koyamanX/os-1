#include <syscall.h>

.text
.globl read
read:
	li a7, __NR_READ
	ecall
	ret
