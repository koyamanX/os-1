#ifndef _WAIT_H
#define _WAIT_H

#define WNOHANG 1

int waitpid(int pid, int *status, int options);

#endif
