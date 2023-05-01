#include <syscall.h>

.text
.globl _exit
_exit:
	li a7, __NR__EXIT
	ecall
	ret
