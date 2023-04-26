#ifndef _SLEEPLOCK_H
#define _SLEEPLOCK_H

struct sleeplock {
    int locked;
    int pid;
    char *name;
};

void sleep_lock_init(struct sleeplock *lk, char *name);
void sleep_lock(struct sleeplock *lk);
void sleep_unlock(struct sleeplock *lk);
int sleep_is_locked(struct sleeplock *lk);

#endif /* _SLEEPLOCK_H */
