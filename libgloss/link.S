#include <syscall.h>

.text
.globl _link
_link:
	li a7, __NR_LINK
	ecall
	ret
