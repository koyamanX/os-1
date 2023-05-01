#include <syscall.h>

.text
.globl _mkdir
_mkdir:
	li a7, __NR_MKDIR
	ecall
	ret
