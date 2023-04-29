/**
 * @file proc.h
 * @brief Structures and function prototypes for process management.
 * @author ckoyama(koyamanX)
 */

#ifndef _PROC_H
#define _PROC_H

#include <file.h>
#include <riscv.h>

/**
 * @brief Struct for trapframe.
 * @details It is used for saving context of interrupted process.
 */
typedef struct {
    u64 ra;
    u64 sp;
    u64 gp;
    u64 tp;
    u64 t0;
    u64 t1;
    u64 t2;
    u64 s0;
    u64 s1;
    u64 a0;
    u64 a1;
    u64 a2;
    u64 a3;
    u64 a4;
    u64 a5;
    u64 a6;
    u64 a7;
    u64 s2;
    u64 s3;
    u64 s4;
    u64 s5;
    u64 s6;
    u64 s7;
    u64 s8;
    u64 s9;
    u64 s10;
    u64 s11;
    u64 t3;
    u64 t4;
    u64 t5;
    u64 t6;
    u64 sepc;          //!< supervisour exception PC, trapped address.
    u64 satp;          //!< satp of a process.
    u64 ksp;           //!< stack pointer for per-process kernel stack.
    u64 trap_handler;  //!< Supervisor mode trap handler.
} trapframe_t;

/**
 * @brief Structure for execution context of process.
 * @details It is used for saving callee-saved registers.
 */
typedef struct {
    u64 ra;
    u64 sp;
    u64 s0;
    u64 s1;
    u64 s2;
    u64 s3;
    u64 s4;
    u64 s5;
    u64 s6;
    u64 s7;
    u64 s8;
    u64 s9;
    u64 s10;
    u64 s11;
} context_t;

#define NPROCS 16          //!< Maximum number of process.
#define NOFILE 8           //!< Maximum number of open file per process.
#define USTACK 0x80000000  //!< Initial stack pointer for userland.
#define NUSTACK 16         //!< Number of user stack in page.

/**
 * @brief Process structure.
 * @details Structure for process.
 */
struct proc {
    u64 stat;                    //!< Execution status of process.
    u64 pid;                     //!< Process IDs.
    trapframe_t *tf;             //!< Trapframe.
    context_t ctx;               //!< Context.
    char name[16];               //!< Name of process.
    struct file *ofile[NOFILE];  //!< Open files.
    pagetable_t pgtbl;           //!< Pagetable of process.
    u64 heap;                    //!< Start address of heap.
    u8 *kstack;                  //!< Pointer to per-process kernel stack.
    void *wchan;                 //!< Waiting channel.
    u64 ppid;                    //!< Parent process.
};

#define UNUSED 0    //!< Proc struct is unused.
#define USED 1      //!< Proc struct is used.
#define RUNNING 2   //!< Proc is running.
#define RUNNABLE 3  //!< Proc is runnable.
#define ZOMBIE 4    //!< Proc is zombie.
#define SLEEP 5     //!< Proc is sleeping.

/**
 * @brief Struct for Processors.
 * @details It contains running process on a processor and scheduler's context.
 */
struct cpu {
    struct proc *rp;  //!< Running process.
    context_t ctx;    //!< Context of scheduler.
};
extern struct cpu cpu;  //!< Processors kernel can run.

#define this_cpu() (cpu)      //!< This processor's corresponding cpu struct.
#define this_proc() (cpu.rp)  //!< This process's corresponding proc struct.

extern struct proc procs[NPROCS];  //!< Processes.

/**
 *	@brief Initialize proc structure.
 *	@details Initialize proc structure.
 */
void initproc(void);
/**
 *	@brief Initialize cpu structure.
 *	@details Initialize cpu structure.
 */
void initcpu(void);
/**
 * @brief Initialize vritual memory for first user process, init.
 * @return Pointer to pagetable.
 */
pagetable_t uvminit(void);
/**
 * @brief Allocate new proc structure and initialize.
 * @details Allocate new proc structure and initialize.
 * @return newly created proc.
 */
struct proc *newproc(void);
/**
 * @brief Allocate user page for first user-mode process, init
 * @details Allocate user page for first user-mode process, init
 */
void userinit(void);

/**
 * @brief Execute a file.
 * @details Execute a file, load process image.
 * @param[in] file Pointer to path of file.
 * @param[in] argv Pointer to argument vector.
 * @return 0 on success, -1 on failure.
 */
int execv(const char *file, char const **argv);

/**
 * @brief Exit process.
 * @details Exit process, free memory, proc struct, etc, and set status as exit
 * status.
 * @param[in] status exit status.
 */
void _exit(int status);

/**
 * @brief Sleep for wait channel.
 * @details Sleep for wait channel, process will be SLEEP.
 * @param[in] wchan address to wait for.
 */
void sleep(void *wchan);

/**
 * @brief Wakeup process waitting for wchan.
 * @details Wakeup process waitting for wchan, set RUNNABLE.
 * @param[in] wchan address to wakeup.
 */
void wakeup(void *wchan);

/**
 * @brief switch context.
 * @details swtich context, used for switching process and scheduler, written in
 * asm.
 * @param[in] old old context.
 * @param[in] new new context.
 */
extern void swtch(context_t *old, context_t *new);

#endif  // _PROC_H
