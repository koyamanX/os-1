#include <syscall.h>

.text
.globl open
open:
	li a7, __NR_OPEN
	ecall
	ret
