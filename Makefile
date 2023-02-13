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

kernel: start.S kernel.ldS main.c vm.c uart.c string.c printk.c timer.c trap.c proc.c panic.c sched.c swtch.S init.S virtio.c fs.c
	$(CC) $(CFLAGS) -c start.S
	$(CC) $(CFLAGS) -c init.S
	$(CC) $(CFLAGS) -c virtio.c
	$(CC) $(CFLAGS) -c swtch.S
	$(CC) $(CFLAGS) -c main.c
	$(CC) $(CFLAGS) -c uart.c
	$(CC) $(CFLAGS) -c vm.c
	$(CC) $(CFLAGS) -c string.c
	$(CC) $(CFLAGS) -c printk.c
	$(CC) $(CFLAGS) -c timer.c
	$(CC) $(CFLAGS) -c trap.c
	$(CC) $(CFLAGS) -c proc.c
	$(CC) $(CFLAGS) -c panic.c
	$(CC) $(CFLAGS) -c sched.c
	$(CC) $(CFLAGS) -c fs.c
	$(LD) $(LDFLAGS) vm.o panic.o proc.o start.o uart.o main.o string.o printk.o timer.o trap.o sched.o swtch.o init.o virtio.o fs.o -o kernel -T kernel.ldS
	$(OBJDUMP) -D kernel > kernel.dump

rootfs.img:
	fallocate -l 512M rootfs.img
	mkfs.minix -3 rootfs.img
	mkdir ./os1
	sudo mount rootfs.img ./os1
	touch os1/hello.txt
	sync
	sudo umount ./os1

QEMU=qemu-system-riscv64

qemu: kernel rootfs.img
	$(QEMU) -D qemu.log -d in_asm -machine virt -bios none -kernel kernel -m 128M -nographic -global virtio-mmio.force-legacy=false -drive file=rootfs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0

qemu-gdb: kernel
	$(QEMU) -D qemu.log -d in_asm -machine virt -bios none -kernel kernel -m 128M -nographic -S -gdb tcp::12345 -global virtio-mmio.force-legacy=false -drive file=rootfs.img,if=none,format=raw,id=x0 -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0


clean:
	sudo umount os1
	rm -rf *.o kernel *.dump *.log rootfs.img os1
