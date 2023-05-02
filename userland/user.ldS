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

	. = ALIGN(16);
	_bss_start = .;
	.bss : {
		. = ALIGN(16);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
	}
	. = ALIGN(16);
	_bss_end = .;
	. = ALIGN(0x1000);
	. = 0x20000000;
	_heap_start = .;

	/DISCARD/ : {*(*) }

	_stack_top = 0x80000000;
}
