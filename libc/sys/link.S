#include <syscall.h>

.text
.globl link
link:
	li a7, __NR_LINK
	ecall
	ret
