#include <proc.h>
#include <syscall.h>
#include <vm.h>
#include <panic.h>
#include <stddef.h>
#include <uart.h>
#include <fcntl.h>
#include <sys/stat.h>

ssize_t write(int fd, const void *buf, size_t count);
int syscall(struct proc *rp) {
	u64 syscall_num = rp->tf->a7;
	u64 a0 = rp->tf->a0;
	u64 a1 = rp->tf->a1;
	u64 a2 = rp->tf->a2;
	int ret = -1;

	switch(syscall_num) {
		case __NR_WRITE:
			ret = write(a0, (void *)va2pa(rp->pgtbl, a1), a2);
			break;
		case __NR_EXECEV:
			ret = exec(((const char *)va2pa(rp->pgtbl, a0)), ((char const **)va2pa(rp->pgtbl, a1)));
			break;
		case __NR_OPEN:
			ret = open(((const char *)va2pa(rp->pgtbl, a0)), a1, a2);
			break;
		case __NR_MKDIR:
			ret = mkdir(((const char *)va2pa(rp->pgtbl, a0)), a1);
			break;
		default:
			panic("invalid syscall\n");
			break;
	}

	return ret;
}

ssize_t write(int fd, const void *buf, size_t count) {
	u64 i = 0;
	if(fd != 1) {
		return -1;
	}
	for(i = 0; i < count; i++) {
		uart_putchar(((u8 *)buf)[i]);
	}
	return 0;
}
