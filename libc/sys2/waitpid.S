#include <syscall.h>

.text
.globl waitpid
waitpid:
	li a7, __NR_WAITPID
	ecall
	ret
