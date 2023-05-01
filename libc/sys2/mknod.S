#include <syscall.h>

.text
.globl mknod
mknod:
	li a7, __NR_MKNOD
	ecall
	ret
