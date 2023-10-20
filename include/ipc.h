#ifndef IPC_H
#define IPC_H

#include <riscv.h>

/**
 * IPC in OS-1 has following features.
 * - Direct IPC
 * - Synchronous communication
 * - No-timeout
 * - Communication types of
 *   - Send
 *   - Recv
 *   - Send and Recv
 *   - Reply and Recv
 *   - Notifications
 */

#define IPC_SEND (1 << 0)
#define IPC_RECV (1 << 1)
#define IPC_REPLY (1 << 2)
#define IPC_NOTIFY (1 << 3)
#define IPC_SENDRECV (IPC_SEND | IPC_RECV)
#define IPC_REPLYRECV (IPC_REPLY | IPC_RECV)

#if 0
#define ipc_send(dst, msg) ipc(dst, msg, IPC_SEND)
#define ipc_recv(src, msg) ipc(src, msg, IPC_RECV)
#define ipc_sendrecv(src, msg) ipc(src, msg, IPC_SENDRECV)
#define ipc_replyrecv(src, msg) ipc(src, msg, IPC_REPLYRECV)
#endif

// endpoint_t is basically a pid_t.
typedef int endpoint_t;

#define IPC_ANY ((endpoint_t)-1)	// Any endpoint.
#define IPC_NONE ((endpoint_t)-2)	// No endpoint.

/**
* IPC message structure.
* This is used for both sending and receiving.
* Total size of message is a 1KB size.
*/
typedef struct message {
	int type;
	endpoint_t src;
	union {
		char *m0;
		char m1[1024 - sizeof(int) - sizeof(endpoint_t)];
	} m;
} message_t;

#define m0 m.m0
#define m1 m.m1

int ipc(endpoint_t ep, message_t *msg, int type);

#endif
