#include <syscall.h>

.text
.section .userproc0
.globl init
init:
	li a7, __NR_EXECEV
	la a0, file
	li a1, 0
	ecall

loop:
	j loop	
file:
.string "/usr/sbin/init"
