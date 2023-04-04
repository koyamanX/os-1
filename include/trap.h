#ifndef TRAP_H
#define TRAP_H

void kerneltrap(void);
extern void usertrapret(void);
extern void userret(u64 satp);

#endif
