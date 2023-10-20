#include <syscall.h>

.text
.globl ipc
ipc:
	li a7, __NR_IPC
	ecall
	ret