.globl _start
_start:
.extern main
	call main
exit:
	j exit
