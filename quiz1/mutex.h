#pragma once

#if USE_PTHREADS

#include <pthread.h>

#define mutex_t pthread_mutex_t
#define mutex_init(m) pthread_mutex_init(m, NULL)
#define MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define mutex_trylock(m) (!pthread_mutex_trylock(m))
#define mutex_lock pthread_mutex_lock
#define mutex_unlock pthread_mutex_unlock
#define mutex_destroy pthread_mutex_destroy
#define mutex_trylock_pi(m) (!pthread_mutex_trylock(m))
#define mutex_lock_pi pthread_mutex_lock
#define mutex_unlock_pi pthread_mutex_unlock

#define mutexattr_t pthread_mutexattr_t
#define mutexattr_init pthread_mutexattr_init
#define mutexattr_destroy pthread_mutexattr_destroy
#define mutexattr_setprotocol pthread_mutexattr_setprotocol
#define PRIO_INHERIT PTHREAD_PRIO_INHERIT
#define PRIO_NONE PTHREAD_PRIO_NONE

#else

#include <stdbool.h>
#include "atomic.h"
#include "futex.h"
#include "spinlock.h"

typedef struct {
    atomic int state;
} mutex_t;

typedef struct {
    int protocol;
} mutexattr_t;

enum {
    MUTEX_LOCKED = 1 << 0,
    MUTEX_SLEEPING = 1 << 1,
};

enum {
    PRIO_NONE = 0,
    PRIO_INHERIT = 1 << 0,
};

#define MUTEX_INITIALIZER \
    {                     \
        .state = 0        \
    }

static inline void mutex_init(mutex_t *mutex)
{
    atomic_init(&mutex->state, 0);
}

static bool mutex_trylock(mutex_t *mutex)
{
    int state = load(&mutex->state, relaxed);
    if (state & MUTEX_LOCKED)
        return false;

    state = fetch_or(&mutex->state, MUTEX_LOCKED, relaxed);
    if (state & MUTEX_LOCKED)
        return false;

    thread_fence(&mutex->state, acquire);
    return true;
}

static inline void mutex_lock(mutex_t *mutex)
{
#define MUTEX_SPINS 128
    for (int i = 0; i < MUTEX_SPINS; ++i) {
        if (mutex_trylock(mutex))
            return;
        spin_hint();
    }

    int state = exchange(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING, relaxed);

    while (state & MUTEX_LOCKED) {
        futex_wait(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING);
        state = exchange(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING, relaxed);
    }

    thread_fence(&mutex->state, acquire);
}

static inline void mutex_unlock(mutex_t *mutex)
{
    int state = exchange(&mutex->state, 0, release);
    if (state & MUTEX_SLEEPING)
        futex_wake(&mutex->state, 1);
}

static bool mutex_trylock_pi(mutex_t *mutex)
{
    return true;
}
static inline void mutex_lock_pi(mutex_t *mutex) {}
static inline void mutex_unlock_pi(mutex_t *mutex) {}

/* dummy */
static inline void mutex_destroy(mutex_t *mutex) {}
static inline void mutex_attr_init(mutexattr_t *attr) {}
static inline void mutex_attr_destroy(mutexattr_t *attr) {}
static inline void mutex_attr_setprotocol(mutexattr_t *attr, int protocol) {}

#endif
