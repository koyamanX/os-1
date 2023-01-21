PREFIX=riscv64-linux-gnu-
CC=$(PREFIX)gcc-12
AS=$(PREFIX)as
LD=$(PREFIX)ld
OBJCOPY=$(PREFIX)objcopy
OBJDUMP=$(PREFIX)objdump

CFLAGS  = -Wall -Werror -O0 -ggdb -gdwarf-2
CFLAGS += -march=rv64g
CFLAGS += -ffreestanding -fno-common -nostdlib 
CFLAGS += -fno-omit-frame-pointer -mno-relax -mcmodel=medany -fno-stack-protector 
CFLAGS += -fno-pie -no-pie 
#CFLAGS +=  -no-pie 

LDFLAGS = -z max-page-size=4096

kernel: start.S kernel.ldS main.c
	$(CC) $(CFLAGS) -c start.S
	$(CC) $(CFLAGS) -c main.c
	$(LD) $(LDFLAGS) start.o main.o -o kernel -T kernel.ldS
	$(OBJDUMP) -D kernel > kernel.dump

QEMU=qemu-system-riscv64

qemu: kernel
	$(QEMU) -D qemu.log -d in_asm -machine virt -bios none -kernel kernel -m 128M -nographic

qemu-gdb: kernel
	$(QEMU) -D qemu.log -d in_asm -machine virt -bios none -kernel kernel -m 128M -nographic -S -gdb tcp::12345

clean:
	rm -rf *.o kernel *.dump *.log
