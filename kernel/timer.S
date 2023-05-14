.text
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

