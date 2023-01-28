.section .text.init
.align 4
.globl _start

_start:
	# clear bss
	la t0, _bss_start
	la t1, _bss_end
_clear_bss:
	sd zero, 0(t0)
	add t0, t0, 8
	bltu t0, t1, _clear_bss

	# Set mpp to s-mode
	li t0, 0x3<<11
	csrs mstatus, t0

	# delegate all traps to s-mode
	li t0, -1
	csrw mideleg, t0
	csrw medeleg, t0

	# Allow s-mode to access all memory region
	li t0, 0x3fffffffffffff
	li t1, 0xf
	csrw pmpaddr0, t0
	csrw pmpcfg0, t1

	# Set mepc to kinit
	la t0, kinit
	csrw mepc, t0
	# Setup stack(4KB)
	la sp, stack
	li t0, 4096
	add sp, sp, t0
	
	# Set tp to mhartid
	csrr tp, mhartid
	# call main with s-mode
	mret

_spin:
	j _spin


.globl timervec
timervec:
	csrrw a0, mscratch, a0
	sd a1, 0x00(a0)
	sd a2, 0x08(a0)
	sd a3, 0x10(a0)

	# Interval
	ld a1, 0x20(a0)
	# mtimecmp
	ld a2, 0x18(a0)
	ld a3, 0x00(a2)
	# mtimecmp + interval
	add a1, a1, a3
	# update mtimecmp
	sd a1, 0x00(a2)

	li a1, 0x02
	csrw sip, a1

	ld a1, 0x00(a0)
	ld a2, 0x08(a0)
	ld a3, 0x10(a0)
	csrrw a0, mscratch, a0

	mret

.globl kernelvec
kernelvec:
	addi sp, sp, -256
	sd x1, 0(sp)
	sd x2, 8(sp)
	sd x3, 16(sp)
	sd x4, 24(sp)
	sd x5, 32(sp)
	sd x6, 40(sp)
	sd x7, 48(sp)
	sd x8, 56(sp)
	sd x9, 64(sp)
	sd x10, 72(sp)
	sd x11, 80(sp)
	sd x12, 88(sp)
	sd x13, 96(sp)
	sd x14, 104(sp)
	sd x15, 112(sp)
	sd x16, 120(sp)
	sd x17, 128(sp)
	sd x18, 136(sp)
	sd x19, 144(sp)
	sd x20, 152(sp)
	sd x21, 160(sp)
	sd x22, 168(sp)
	sd x23, 176(sp)
	sd x24, 184(sp)
	sd x25, 192(sp)
	sd x26, 200(sp)
	sd x27, 208(sp)
	sd x28, 216(sp)
	sd x29, 224(sp)
	sd x30, 232(sp)
	sd x31, 240(sp)

	call kerneltrap

	ld x1, 0(sp)
	ld x2, 8(sp)
	ld x3, 16(sp)
	ld x4, 24(sp)
	ld x5, 32(sp)
	ld x6, 40(sp)
	ld x7, 48(sp)
	ld x8, 56(sp)
	ld x9, 64(sp)
	ld x10, 72(sp)
	ld x11, 80(sp)
	ld x12, 88(sp)
	ld x13, 96(sp)
	ld x14, 104(sp)
	ld x15, 112(sp)
	ld x16, 120(sp)
	ld x17, 128(sp)
	ld x18, 136(sp)
	ld x19, 144(sp)
	ld x20, 152(sp)
	ld x21, 160(sp)
	ld x22, 168(sp)
	ld x23, 176(sp)
	ld x24, 184(sp)
	ld x25, 192(sp)
	ld x26, 200(sp)
	ld x27, 208(sp)
	ld x28, 216(sp)
	ld x29, 224(sp)
	ld x30, 232(sp)
	ld x31, 240(sp)

	addi sp, sp, 256

	sret

.globl trampoline
trampoline:
	csrrw a0, sscratch, a0
	li a0, 0x7fffffc000

	sd x1, 0(a0)
	sd x2, 8(a0)
	sd x3, 16(a0)
	sd x4, 24(a0)
	sd x5, 32(a0)
	sd x6, 40(a0)
	sd x7, 48(a0)
	sd x8, 56(a0)
	sd x9, 64(a0)
	sd x11, 80(a0)
	sd x12, 88(a0)
	sd x13, 96(a0)
	sd x14, 104(a0)
	sd x15, 112(a0)
	sd x16, 120(a0)
	sd x17, 128(a0)
	sd x18, 136(a0)
	sd x19, 144(a0)
	sd x20, 152(a0)
	sd x21, 160(a0)
	sd x22, 168(a0)
	sd x23, 176(a0)
	sd x24, 184(a0)
	sd x25, 192(a0)
	sd x26, 200(a0)
	sd x27, 208(a0)
	sd x28, 216(a0)
	sd x29, 224(a0)
	sd x30, 232(a0)
	sd x31, 240(a0)

	csrr t0, sscratch
	sd t0, 72(a0)

	csrr t1, sepc
	sd t1, 248(a0)

	ld t2, 256(a0)
	csrr t3, satp
	sd t3, 256(a0)

	la sp, stack

.globl userret
userret:
	ld t2, 256(a0)
	sfence.vma
	csrw satp, t2
	sfence.vma

	csrrw a0, sscratch, a0
	li a0, 0x7fffffc000

	ld t1, 248(a0)
	csrw sepc, t1

	ld x1, 0(a0)
	ld x2, 8(a0)
	ld x3, 16(a0)
	ld x4, 24(a0)
	ld x5, 32(a0)
	ld x6, 40(a0)
	ld x7, 48(a0)
	ld x8, 56(a0)
	ld x9, 64(a0)
	ld x11, 80(a0)
	ld x12, 88(a0)
	ld x13, 96(a0)
	ld x14, 104(a0)
	ld x15, 112(a0)
	ld x16, 120(a0)
	ld x17, 128(a0)
	ld x18, 136(a0)
	ld x19, 144(a0)
	ld x20, 152(a0)
	ld x21, 160(a0)
	ld x22, 168(a0)
	ld x23, 176(a0)
	ld x24, 184(a0)
	ld x25, 192(a0)
	ld x26, 200(a0)
	ld x27, 208(a0)
	ld x28, 216(a0)
	ld x29, 224(a0)
	ld x30, 232(a0)
	ld x31, 240(a0)

	sret

