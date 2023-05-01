#include <syscall.h>

.text
.globl mkdir
mkdir:
	li a7, __NR_MKDIR
	ecall
	ret
