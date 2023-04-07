OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS {
	. = 0x1000;

	.text : {
		*(.text .text.*)
	}
	
	.rodata : {
		. = ALIGN(16);
		*(.srodata .srodata.*)
		. = ALIGN(16);
		*(.rodata .rodata.*)
	}
	
	.data : {
		. = ALIGN(16);
		*(.sdata .sdata.*)
		. = ALIGN(16);
		*(.data .data.*)
	}

	.bss : {
		. = ALIGN(16);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
	}

	.note : {
		KEEP(*(.note.gnu.build-id))
	}

	PROVIDE(_end = .);
}
