.text
.align 0xc
.globl trampoline
trampoline:
	csrrw a0, sscratch, a0
	li a0, 0x3fffffe000

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

	ld sp, 264(a0)

	ld t5, 272(a0)

	sfence.vma
	csrw satp, t2
	sfence.vma

	jr t5

.globl userret
userret:
	sfence.vma
	csrw satp, a0
	sfence.vma

	li a0, 0x3fffffe000

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
	ld x10, 72(a0)

	sret

