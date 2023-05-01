#include <lib.h>
#include <proc.h>
#include <vm.h>

// TODO: pid_t
int fork(void) {
    struct proc *child;
    struct proc *parent;

    // Allocate new proc.
    child = newproc();
    if ((child == NULL)) {
        return -1;
    }
    // Get parent proc.
    parent = this_proc();
    // Set parent pid.
    child->ppid = parent->pid;
    // Copy trapframe and kernel stack.
    memcpy(child->tf, parent->tf, sizeof(trapframe_t));
    memcpy(child->kstack, parent->kstack, PAGE_SIZE);
    // Set return value for child.
    child->tf->a0 = 0;
    // Copy memory space.
    uvmcopy(child->pgtbl, parent->pgtbl, 0x80000000);
    // Copy open files.
    memcpy((char *)child->ofile, (char *)parent->ofile, sizeof(parent->ofile));

    return child->pid;
}
