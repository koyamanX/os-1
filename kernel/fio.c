#include <file.h>
#include <stddef.h>
#include <proc.h>
#include <riscv.h>
#include <panic.h>

struct file file[NFILE];

static int ufalloc(void) {
	struct proc *rp;

	rp = cpus[r_tp()].rp;

	for(int i = 0; i < NOFILE; i++) {
		if(rp->ofile[i] == NULL) {
			return i;
		}
	}
	return -1;
}

struct file *falloc(void) {
	int fd;
	struct proc *rp;
	struct file *fp;

	rp = cpus[r_tp()].rp;
	fd = ufalloc();
	
	if(fd < 0) {
		return NULL;
	}

	for(int i = 0; i < NFILE; i++) {
		if(file[i].count == 0) {
			fp = &file[i];
			rp->ofile[fd] = fp;
			fp->count++;
			fp->offset[0] = 0;
			fp->offset[1] = 0;
			fp->ip = NULL;
			return fp;
		}
	}
	panic("no file\n");
	return NULL;
}