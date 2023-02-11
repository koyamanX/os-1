.text
.section .userproc0
.globl init
init:
	li a7, 1
	li a0, 1
	la a1, hello
	la a2, 6

	ecall
	li a7, 1
	li a0, 1
	la a1, world
	la a2, 6
	ecall

	li a7, 1
	li a0, 1
	la a1, newline
	la a2, 1
	ecall

loop:
	j loop	

hello:
.string "hello,"
world:
.string "world!"
newline:
.string "\n"
