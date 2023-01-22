#ifndef RISCV_H
#define RISCV_H

typedef unsigned long long u64;
typedef long long int i64;
typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;

#define MSTATUS_MPP_MASK (0x3<<11)
#define MSTATUS_MPP_M_MODE (0x3<<11)
#define MSTATUS_MPP_S_MODE (0x1<<11)
#define MSTATUS_MIE			(0x1<<3)

#define MIE_MEIE	(0x1<<11)
#define MIE_MTIE	(0x1<<7)
#define MIE_MSIE	(0x1<<3)

#define SIE_SEIE	(0x1<<9)
#define SIE_STIE	(0x1<<5)
#define SIE_SSIE	(0x1<<1)

#define CLINT_BASE		0x02000000
#define CLINT_MTIMECMP	(CLINT_BASE+0x4000)
#define CLINT_MTIME		(CLINT_BASE+0xBFF8)

#define NULL ((void *)0)
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
#define TRAMPOLINE	(((u64)2<<(39-1))-PAGE_SIZE)
#define TRAPFRAME	(TRAMPOLINE-PAGE_SIZE)

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

#define PTE_V		(1<<0)
#define PTE_R		(1<<1)
#define PTE_W		(1<<2)
#define PTE_X		(1<<3)
#define PTE_U		(1<<4)
#define PTE_G		(1<<5)
#define PTE_A		(1<<6)
#define PTE_D		(1<<7)

typedef u64 * pagetable_t;
typedef u64 pte_t;

struct page {
	struct page *next;
};
struct kmem {
	struct page *freelist;
	// TODO: lock
};

#define SV39 (8UL << 60)
#define SATP(pgtbl) (SV39 | ((u64)pgtbl) >> 12)

#define INSTRUCTION_PAGE_FAULT 12
#define LOAD_PAGE_FAULT 13
#define STORE_AMO_PAGE_FAULT 15
#define LOAD_ACCESS_FAULT 5

static inline u64 r_mstatus(void) {
	u64 v;
	asm volatile("csrr %0, mstatus" : "=r"(v));
	return v;
}
static inline void w_mstatus(u64 v) {
	asm volatile("csrw mstatus, %0" : : "r"(v));
}
static inline u64 r_mie(void) {
	u64 v;
	asm volatile("csrr %0, mie" : "=r"(v));
	return v;
}
static inline void w_mie(u64 v) {
	asm volatile("csrw mie, %0" : : "r"(v));
}
static inline void w_mepc(u64 v) {
	asm volatile("csrw mepc, %0" : : "r"(v));
}
static inline void w_mtvec(u64 v) {
	asm volatile("csrw mtvec, %0" : : "r"(v));
}
static inline u64 r_mscratch(void) {
	u64 v;
	asm volatile("csrr %0, mscratch" : "=r"(v));
	return v;
}
static inline void w_mscratch(u64 v) {
	asm volatile("csrw mscratch, %0" : : "r"(v));
}

static inline void w_satp(u64 v) {
	asm volatile("csrw satp, %0" : : "r"(v));
}
static inline void w_sie(u64 v) {
	asm volatile("csrw sie, %0" : : "r"(v));
}
static inline void w_sip(u64 v) {
	asm volatile("csrw sip, %0" : : "r"(v));
}
static inline u64 r_sstatus(void) {
	u64 v;
	asm volatile("csrr %0, sstatus" : "=r"(v));
	return v;
}
static inline void w_sstatus(u64 v) {
	asm volatile("csrw sstatus, %0" : : "r"(v));
}
static inline u64 r_sscratch(void) {
	u64 v;
	asm volatile("csrr %0, sscratch" : "=r"(v));
	return v;
}
static inline u64 r_scause(void) {
	u64 v;
	asm volatile("csrr %0, scause" : "=r"(v));
	return v;
}
static inline void w_sscratch(u64 v) {
	asm volatile("csrw sscratch, %0" : : "r"(v));
}
static inline void w_stvec(u64 v) {
	asm volatile("csrw stvec, %0" : : "r"(v));
}
static inline void sfence_vma(void) {
	asm volatile("sfence.vma");
}

#endif
