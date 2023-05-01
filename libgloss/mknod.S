#include <syscall.h>

.text
.globl _mknod
_mknod:
	li a7, __NR_MKNOD
	ecall
	ret
