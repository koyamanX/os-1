#include "riscv.h"

__attribute__ ((aligned (16))) char stack[4096];

extern char *_end;

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

void kinit(void) {
	u64 mstatus;

	mstatus = r_mstatus();
	mstatus &= MSTATUS_MPP_MASK;
	mstatus |= MSTATUS_MPP_S_MODE;
	w_mstatus(mstatus);

	w_satp(0);

	//init_timer();
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

#define UART_BASE	0x10000000

#define UART_RBR	((volatile u8*)UART_BASE+0x0)
#define UART_THR	((volatile u8*)UART_BASE+0x0)
#define UART_DLL	((volatile u8*)UART_BASE+0x0)
#define UART_DLM	((volatile u8*)UART_BASE+0x1)
#define UART_IER	((volatile u8*)UART_BASE+0x1)
#define UART_IIR	((volatile u8*)UART_BASE+0x2)
#define UART_FCR	((volatile u8*)UART_BASE+0x2)
#define UART_LCR	((volatile u8*)UART_BASE+0x3)
#define UART_MCR	((volatile u8*)UART_BASE+0x4)
#define UART_LSR	((volatile u8*)UART_BASE+0x5)
#define UART_MSR	((volatile u8*)UART_BASE+0x6)
#define UART_SCR	((volatile u8*)UART_BASE+0x7)

#define UART_LCR_DLAB	(1<<7)
#define UART_LCR_THRE   (1<<5)

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

#define PTE_V		(1<<0)
#define PTE_R		(1<<1)
#define PTE_W		(1<<2)
#define PTE_X		(1<<3)
#define PTE_U		(1<<4)
#define PTE_G		(1<<5)
#define PTE_A		(1<<6)
#define PTE_D		(1<<7)

#define PAGE_OFFSET	12
#define PAGE_SIZE	(1<<PAGE_OFFSET)
#define ROUNDUP(x) 		((((u64)(x)) + PAGE_SIZE-1) & (~(PAGE_SIZE-1)))
#define ROUNDDOWN(x)	(((u64)(x)) & (~(PAGE_SIZE-1)))

#define PA2PTE(pa)		(((u64)(pa) >> 12) << 10)
#define PTE2PA(pte)		(((u64)(pte) >> 10) << 12)
#define PTE_FLAGS(pte)	((pte) & 0x3ff)

#define VAIDX_MASK		0x1ff
#define VA_LEVEL(level) (9*(level))
#define VA2IDX_SHIFT(level) (PAGE_OFFSET + (VA_LEVEL(level)))
#define VA2IDX(va, level) ((((u64)(va)) >> VA2IDX_SHIFT(level)) & VAIDX_MASK)

// 128MB
#define PHYEND	((u64 *) 0x88000000)

struct page {
	struct page *next;
};
struct kmem {
	struct page *freelist;
	// TODO: lock
} kmem;

void *memset(void *s, int c, u64 sz) {
	char *p = (char *)s;

	for(u64 i = 0; i < sz; i++) {
		p[i] = c;
	}
	return s;
}

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

void kvminit(void) {
	kmeminit();	
}

void kmain(void) {
	uart_init();
	uart_puts("hello,world\n");
	kvminit();

	while(1) {
		asm volatile("nop");
	}
}
void kerneltrap(void) {
	return ;
}
