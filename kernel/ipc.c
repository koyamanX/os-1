#include <ipc.h>
#include <proc.h>
#include <printk.h>
#include <sched.h>

// send a message to a process
static int ipc_send(endpoint_t dst, message_t *msg) {
    // Check if dst is IPC_ANY.
    if (dst == IPC_ANY) {
        // return error if dst is IPC_ANY.
        return -1;
    }

    // get destination process.
    struct proc *dst_proc = &procs[dst-1];

    // check if destination process is not used.
    if (dst_proc->stat == UNUSED) {
        // return error if destination process is not used.
        return -1;
    }

    // check for deadlock.
    // if destination process is blocked and waiting for this process.
    if (dst_proc->stat == SLEEP && (dst_proc->recvfrom == this_proc()->pid || dst_proc->recvfrom == IPC_ANY)) {
        // Destination process is blocked and waiting for this process to send message.
        // Send message then wake up destination process.

        // copy message to destination process.
        dst_proc->msg = *msg;
        // set destination process's state to RUNNABLE.
        wakeup((void *)&dst_proc->msg);
        // clear recvfrom field of destination process.
        dst_proc->recvfrom = IPC_NONE;
        
        // return 0 if message is sent successfully.
        return 0;
    }

    // TODO: if dest is not waitting for this process to send message, then add to waitting queue.
    return -1; // TODO
}

// receive a message from a process
static int ipc_recv(endpoint_t src, message_t *msg) {
    // Get this process.
    struct proc *this_proc = this_proc();

    // Set this process's recvfrom field to src.
    this_proc->recvfrom = src;
    // Block this process.
    sleep((void *)&this_proc->msg);

    // copy received message to this process.
    *msg = this_proc->msg;

    return 0;
}

int ipc(endpoint_t ep, message_t *msg, int type) {
    if(type & IPC_SEND) {
        return ipc_send(ep, msg);
    }
    if(type & IPC_RECV) {
        return ipc_recv(ep, msg);
    }
    return -1;
}