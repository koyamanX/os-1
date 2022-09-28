OUTPUT_ARCH("riscv")
ENTRY(_start)

SECTIONS {
	. = 0x80000000;

	.text : {
		*(.text .text.*)
		. = ALIGN(0x1000);
		PROVIDE(_etext = .);
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
		_bss_start = .;
		. = ALIGN(16);
		*(.sbss .sbss.*)
		. = ALIGN(16);
		*(.bss .bss.*)
		_bss_end = .;
	}

	PROVIDE(_end = .);
}
