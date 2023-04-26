#include <printk.h>
#include <proc.h>
#include <sleeplock.h>

void sleep_lock_init(struct sleeplock *lk, char *name) {
    lk->name = name;
    lk->locked = 0;
    lk->pid = 0;
}

void sleep_lock(struct sleeplock *lk) {
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0) {
        sleep(lk);
    }
    lk->pid = this_proc()->pid;
}

void sleep_unlock(struct sleeplock *lk) {
    __sync_lock_release(&lk->locked);
    lk->pid = 0;
    wakeup(lk);
}

int sleep_is_locked(struct sleeplock *lk) {
    return lk->locked && (lk->pid == this_proc()->pid);
}
