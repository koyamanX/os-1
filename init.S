.text
.section .userproc0
.globl init
init:
	li a7, 221
	la a0, file
	li a1, 0
	ecall

loop:
	j loop	

hello:
.string "hello,"
world:
.string "world!"
newline:
.string "\n"

file:
.string "/usr/sbin/init"
