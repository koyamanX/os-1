#ifndef IPC_H
#define IPC_H

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
 *
 */

typedef struct endpoint_t {
	pid_t pid;
} endpoint_t;

typedef int notification_t;

typedef struct msg_t {
	endpoint_t src,
	msg_type_t type,
	union {
		char data[2048];
	} data;
} msg_t;

int offer(char *service_name, endpoint_t *endpoint);
int stopoffer(char *service_name);
int find(char *service_name, endpoint_t *endpoint);

int send(endpoint_t *endpoint, msg_t *msg);
int recv(endpoint_t *endpoint, msg_t *msg);
int send_recv(endpoint_t *endpoint, msg_t *smsg, msg_t *rmsg);
int reply_recv(endpoint_t *endpoint, msg_t *smsg, msg_t *rmsg);
int send_notifications(endpoint_t *endpoint, notification_t *notfications);

#endif
