#include <syscall.h>

.text
.globl _waitpid
_waitpid:
	li a7, __NR_WAITPID
	ecall
	ret
