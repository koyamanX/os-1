#include <syscall.h>

.text
.globl close
close:
	li a7, __NR_CLOSE
	ecall
	ret
