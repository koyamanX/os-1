.globl _start
_start:
.extern main
	call main
exit:
	mv a0, zero
	j _exit
