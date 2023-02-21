.globl _start
_start:
.extern main
	li a7, 64
	li a0, 1
	la a1, hello
	la a2, 6
	ecall
	call main
exit:
	j exit
hello:
.string "hello\n"
