set confirm off
set architecture riscv:rv64
target remote localhost:12345
symbol-file kernel
set disassemble-next-line auto
set riscv use-compressed-breakpoints yes
