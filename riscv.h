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
