#if 0
#include "newlib.h"
#endif

.section .text.init
.weak _reset_vector
.extern _start
_reset_vector:
	j _start
.weak _trap_vector
.extern trap_handler
_trap_vector:
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
	j trap_handler
.text
.globl _start
.weak _start
_start:
	csrwi mstatus, 0
	la t0, _trap_vector
	csrw mtvec, t0
	la t0, _bss_start	# 4-byte aligned
	la t1, _bss_end		# 4-byte aligned
_clear_bss:
	sw zero, 0(t0)
	add t0, t0, 4
	bltu t0, t1, _clear_bss
.extern main
.extern exit
	li t0, 0
	li t1, 0
	li t2, 0
	la sp, _stack_top
	la ra, exit
	call main
	tail exit

