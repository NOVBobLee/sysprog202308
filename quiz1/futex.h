#pragma once

#include <limits.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <unistd.h>

#define atomic _Atomic

/* Atomically check if '*futex == value', and if so, go to sleep */
static inline void futex_wait(atomic int *futex, int value)
{
    syscall(SYS_futex, futex, FUTEX_WAIT_PRIVATE, value, NULL);
}

/* Wake up 'limit' threads currently waiting on 'futex' */
static inline void futex_wake(atomic int *futex, int limit)
{
    syscall(SYS_futex, futex, FUTEX_WAKE_PRIVATE, limit);
}

/* Wake up 'limit' waiters, and re-queue the rest onto a different futex */
static inline void futex_requeue(atomic int *futex,
                                 int limit,
                                 atomic int *other)
{
    syscall(SYS_futex, futex, FUTEX_REQUEUE_PRIVATE, limit, INT_MAX, other);
}

/* Acquire lock in kernel space with PI support */
static inline void futex_lock_pi(atomic int *futex)
{
    /* val (= 0) is ignored */
    syscall(SYS_futex, futex, FUTEX_LOCK_PI_PRIVATE, 0, NULL);
}

/* Release lock in kernel space with PI support */
static inline void futex_unlock_pi(atomic int *futex)
{
    syscall(SYS_futex, futex, FUTEX_UNLOCK_PI_PRIVATE);
}

/* Try to acquire lock in kernel space with PI support */
static inline long futex_trylock_pi(atomic int *futex)
{
    return syscall(SYS_futex, futex, FUTEX_TRYLOCK_PI_PRIVATE);
}
