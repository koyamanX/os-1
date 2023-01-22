#include "riscv.h"
#include "proc.h"
#include <stdarg.h>

__attribute__ ((aligned (16))) char stack[PAGE_SIZE*4];

extern char *_end;
extern char *_etext;
extern void trampoline(void);

struct {
	u64 tmp0;
	u64 tmp1;
	u64 tmp2;
	u64 *mtimecmp;
	u64 interval;
} mscratch;

void kinit(void);
void kmain(void);
void init_timer(void);
extern void timervec(void);
extern char kernelvec;
int printk(const char *format, ...);

void kinit(void) {
	u64 mstatus;

	mstatus = r_mstatus();
	mstatus &= MSTATUS_MPP_MASK;
	mstatus |= MSTATUS_MPP_S_MODE;
	w_mstatus(mstatus);

	w_satp(0);

//	init_timer();
	w_sie(SIE_SEIE | SIE_STIE | SIE_SSIE);

	w_mepc((u64)((u64 *)&kmain));

	w_stvec((u64)((u64 *)&kernelvec));
	w_sstatus(r_sstatus() | 1<<1);

	asm volatile("mret");
}
void init_timer(void) {
	u64 interval = 1000000;

	*((u64 *)CLINT_MTIMECMP) = *((u64 *)CLINT_MTIME) + interval;
	mscratch.mtimecmp = (u64 *)CLINT_MTIMECMP;
	mscratch.interval = interval;
	
	w_mscratch((u64)((u64 *)&mscratch));
	w_mtvec((u64)((u64 *)&timervec));
	w_mstatus(r_mstatus() | MSTATUS_MIE);
	w_mie(r_mie() | MIE_MTIE);
}

void uart_init(void) {
	// Disable interrupt
	*UART_IER = 0;
	// 38.4 K baud rate
	*UART_LCR = UART_LCR_DLAB;
	*UART_DLL = 0x3;
	*UART_DLM = 0x0;

	// 8 bits char
	*UART_LCR = 0x3;
	// clear and enable FIFO
	*UART_FCR = 0x3;
}

int uart_putchar(int c) {
	while((*UART_LSR & UART_LCR_THRE) == 0) {
		asm volatile("nop");
	}
	*UART_THR = (u8)c;

	return (u8)c;
}
int uart_puts(char *str) {
	while(*str) {
		uart_putchar(*str++);
	}
	return 0;
}

void *memset(void *s, int c, u64 sz) {
	char *p = (char *)s;

	for(u64 i = 0; i < sz; i++) {
		p[i] = c;
	}
	return s;
}

struct kmem kmem;

void kfree(void *pa) {
	struct page *pz;
	
	memset(pa, 0xff, PAGE_SIZE);
	pz = (struct page *)pa;
	pz->next = kmem.freelist;
	kmem.freelist = pz;
}
void kfreerange(void *pa_start, void *pa_end) {
	for(char *p = (char *)ROUNDUP(pa_start); p < (char *)pa_end; p+=PAGE_SIZE) {
		kfree(p);
	}
}
void *kalloc(void) {
	struct page *pz;

	pz = kmem.freelist;
	kmem.freelist = pz->next;

	return (void*)pz;
}

void kmeminit(void) {
	kfreerange(&_end, PHYEND);
}

pte_t *kvmalloc(pagetable_t pgtbl, u64 va) {
	pte_t *pte;

	for(int level = 2; level >  0; level--) {
		pte = &pgtbl[VA2IDX(va, level)];

		if(*pte & PTE_V) {
			pgtbl = (pagetable_t)PTE2PA(*pte);
		} else {
			pgtbl = (pagetable_t)kalloc();
			memset(pgtbl, 0, PAGE_SIZE);
			*pte = PA2PTE(pgtbl) | PTE_V;
		}
	}
	return &pgtbl[VA2IDX(va, 0)];
}

pte_t *kvmwalk(pagetable_t pgtbl, u64 va) {
	pte_t *pte;

	for(int level = 2; level >  0; level--) {
		pte = &pgtbl[VA2IDX(va, level)];

		if(*pte & PTE_V) {
			pgtbl = (pagetable_t)PTE2PA(*pte);
		}
	}
	return &pgtbl[VA2IDX(va, 0)];
}

void kvmmap(pagetable_t pgtbl, u64 va, u64 pa, u64 sz, u64 perm) {
	pte_t *pte;

	for(u64 i = ROUNDDOWN(va); i < ROUNDDOWN(va+sz); i+=PAGE_SIZE, pa+=PAGE_SIZE) {
		pte = kvmalloc(pgtbl, i);

		*pte = PA2PTE(pa) | perm;
	}
}

void kvmdump(pagetable_t pgtbl, u64 va) {
	pte_t *pte;

	pte = kvmwalk(pgtbl, va);
	if(*pte == 0) {
		printk("%x: not mapped\n", va);
		return ;
	}
	printk("%x: %p, attrs:%x\n", va, PTE2PA(*pte), PTE_FLAGS(*pte));
}

pagetable_t kvminit(void) {
	pagetable_t kpgtbl;

	kmeminit();	
	kpgtbl = kalloc();
	memset(kpgtbl, 0, PAGE_SIZE);
	kvmmap(kpgtbl, (u64)TRAMPOLINE, (u64)trampoline, PAGE_SIZE, PTE_R|PTE_X|PTE_V);
	kvmmap(kpgtbl, 0x80000000, 0x80000000, (u64)&_etext-0x80000000, PTE_R|PTE_X|PTE_V);
	kvmmap(kpgtbl, (u64)&_etext, (u64)&_etext, (u64)PHYEND-(u64)&_etext, PTE_R|PTE_V|PTE_W|PTE_A|PTE_D);
	kvmmap(kpgtbl, UART_BASE, UART_BASE, PAGE_SIZE, PTE_W|PTE_R|PTE_V|PTE_A|PTE_D);

	return kpgtbl;
}

void kvmstart(pagetable_t kpgtbl) {
	sfence_vma();
	w_satp(SATP(kpgtbl));
	sfence_vma();
}

struct proc procs[NPROCS];
u64 mpid = 1;

void initproc(void) {
	for(int i = 0; i < NPROCS; i++) {
		procs[i].stat = UNUSED;
	}
}

char *strcpy(char *dest, const char *src) {
	char *p = dest;

	while(*src) {
		*dest++ = *src++;
	}
	return p;
}

void init(void) {
	while(1) {
		asm volatile("nop");
	}
}

pagetable_t uvminit(void) {
	pagetable_t upgtbl;

	upgtbl = kalloc();
	memset(upgtbl, 0, PAGE_SIZE);
	kvmmap(upgtbl, (u64)TRAMPOLINE, (u64)trampoline, PAGE_SIZE, PTE_R|PTE_X|PTE_V);
	kvmmap(upgtbl, (u64)TRAPFRAME, (u64)kalloc(), PAGE_SIZE, PTE_R|PTE_W|PTE_V);
	kvmmap(upgtbl, (u64)0x0, (u64)init, PAGE_SIZE, PTE_R|PTE_W|PTE_X|PTE_V);

	return upgtbl;
}

void create_init(void) {
	procs[mpid].pid = mpid;
	procs[mpid].stat = RUNNABLE;
	strcpy(procs[mpid].name, "init");
	memset(&procs[mpid].context, 0x0, 64*32);
	procs[mpid].pgtbl = uvminit();
	pte_t *pte = kvmwalk(procs[mpid].pgtbl, TRAPFRAME);
	trapframe_t *tf = (trapframe_t *)PTE2PA(*pte);
	memset(tf, 0, sizeof(trapframe_t));
	tf->satp = SATP(procs[mpid].pgtbl);
}

char *ulltoa(u64 n, char *buffer, int radix) {
	char *p;
	int c = 0;

	p = buffer;
	if(n == 0) {
		buffer[0] = '0';
		buffer[1] = '\0';
		return p;
	}
	while(n > 0) {
		*buffer = "0123456789abcdef"[(n % radix)];
		buffer++;
		n = n / radix;
		c++;
	}
	*buffer = '\0';
	c--;
	for(int i = 0, j = c; i <= c/2; i++, j--) {
		int x;
		x = p[i];
		p[i] = p[j];
		p[j] = x;
	}
	return p;
}

int printk(const char *format, ...) {
	va_list ap;
	const char *bp;
	char buf[256];

	va_start(ap, format);

	bp = format;
	while(*bp) {
		if(*bp == '%') {
			bp++;
			switch(*bp) {
				case 's':
					uart_puts(va_arg(ap, char*));
					bp++;
					break;
				case 'x':
			    case 'p':
					ulltoa(va_arg(ap, u64), buf, 16);
					uart_puts(buf);
					bp++;
					break;
			}
		} else {
			uart_putchar(*bp);
			bp++;
		}
	}

	va_end(ap);
	return 0;
}

void kmain(void) {
	pagetable_t kpgtbl;

	uart_init();
	printk("kernel starts\n");
	kpgtbl = kvminit();
	kvmstart(kpgtbl);
	printk("enable paging\n");
	initproc();
	create_init();

	while(1) {
		asm volatile("nop");
	}
}

void kerneltrap(void) {
	u64 scause = r_scause();
	
	switch(scause) {
		case LOAD_PAGE_FAULT: {
			uart_puts("load page fault\n");
			break;
		}
		case STORE_AMO_PAGE_FAULT: {
			uart_puts("store/amo page fault\n");
			break;
		}
		case INSTRUCTION_PAGE_FAULT: {
			uart_puts("instruction page fault\n");
			break;
		}
		case LOAD_ACCESS_FAULT: {
			uart_puts("load access fault\n");
			break;
		}
		default: {
			uart_puts("fault\n");
			break;
		}
	}

	return ;
}
