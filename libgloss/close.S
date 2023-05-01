#include <syscall.h>

.text
.globl _close
_close:
	li a7, __NR_CLOSE
	ecall
	ret
