#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "atomic.h"
#include "futex.h"
#include "mutex.h"
#include "spinlock.h"  // spin_hint()

#define N_TASKS 3  // H, M, L

struct cs {
    bool h_touched;
    mutex_t lock;
};

struct state {
    atomic int l_locked;
    atomic bool all_created;
    atomic bool stop;

    atomic int n_finished;
    int finished_order[N_TASKS];
};

struct ctx {
    struct cs cs;
    struct state st;
};

void cs_init(struct cs *cs)
{
    mutexattr_t attr;

    mutexattr_init(&attr);
#ifdef USE_PI
    mutexattr_setprotocol(&attr, PRIO_INHERIT);
#endif
    mutex_init(&cs->lock, &attr);
    mutexattr_destroy(&attr);

    cs->h_touched = false;
}

void cs_cleanup(struct cs *cs)
{
    mutex_destroy(&cs->lock);
}

void st_init(struct state *st)
{
    st->l_locked = 0;
    st->all_created = false;
    st->n_finished = 0;
    st->stop = false;
}

void ctx_init(struct ctx *ctx)
{
    cs_init(&ctx->cs);
    st_init(&ctx->st);
}

void ctx_cleanup(struct ctx *ctx)
{
    cs_cleanup(&ctx->cs);
}

enum task_idx {
    TASK_L = 0,
    TASK_M = 1,
    TASK_H = 2,
};

/* put badge on a list in finished-order */
void task_finished(struct state *st, enum task_idx idx)
{
    int order = fetch_add(&st->n_finished, 1, acquire);
    st->finished_order[idx] = order;
}

void print_ctx(struct ctx *ctx)
{
    struct cs *cs = &ctx->cs;
    struct state *st = &ctx->st;

#define PRINT_TASK_ORDER(task_idx)                                             \
    do {                                                                       \
        printf("%s: #%d finished\n", #task_idx, st->finished_order[task_idx]); \
    } while (0)
    PRINT_TASK_ORDER(TASK_L);
    PRINT_TASK_ORDER(TASK_M);
    PRINT_TASK_ORDER(TASK_H);
    printf("h_touched: %s\n", cs->h_touched ? "true" : "false");
}

typedef void *(*mywork_t)(void *);

static void *task_h(void *arg)
{
    struct ctx *ctx = (struct ctx *) arg;
    struct cs *cs = &ctx->cs;
    struct state *st = &ctx->st;

    /* wait until task_l acquired lock */
    if (0 == load(&st->l_locked, acquire))
        futex_wait(&st->l_locked, 0);

    mutex_lock(&cs->lock);

    /* if task_m, task_l are still running */
    if (!load(&st->stop, acquire))
        cs->h_touched = true;

    mutex_unlock(&cs->lock);

    task_finished(st, TASK_H);
    return NULL;
}

static void *task_m(void *arg)
{
    struct ctx *ctx = (struct ctx *) arg;
    struct state *st = &ctx->st;

    /* wait until task_l acquired lock */
    if (0 == load(&st->l_locked, acquire))
        futex_wait(&st->l_locked, 0);

    while (!load(&st->stop, relaxed))
        spin_hint();

    task_finished(st, TASK_M);
    return NULL;
}

static void *task_l(void *arg)
{
    struct ctx *ctx = (struct ctx *) arg;
    struct cs *cs = &ctx->cs;
    struct state *st = &ctx->st;

    mutex_lock(&cs->lock);

    /* wake other tasks if they are waiting for task_l acquired lock */
    store(&st->l_locked, 1, relaxed);
    futex_wake(&st->l_locked, N_TASKS);

    /* yield after all tasks are created */
    while (!load(&st->all_created, acquire))
        spin_hint();
    sched_yield();

    mutex_unlock(&cs->lock);

    task_finished(st, TASK_L);
    return NULL;
}

int main(void)
{
    mywork_t works[N_TASKS] = {task_l, task_m, task_h};
    int priority[N_TASKS] = {5, 10, 15};
    pthread_t th[N_TASKS], self;
    pthread_attr_t attr;
    struct sched_param param;
    struct ctx ctx;

    ctx_init(&ctx);
    pthread_attr_init(&attr);
    pthread_attr_getschedparam(&attr, &param);
    /* use new attr instead of inherited from parent thread attr */
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
    /* run until yield or preempted by higher priority */
    pthread_attr_setschedpolicy(&attr, SCHED_RR);

    /* tune main thread */
    self = pthread_self();
    param.sched_priority = 20;
    pthread_setschedparam(self, SCHED_RR, &param);

    for (int i = 0; i < N_TASKS; ++i) {
        /* set thread priority */
        param.sched_priority = priority[i];
        pthread_attr_setschedparam(&attr, &param);
        pthread_create(&th[i], &attr, works[i], (void *) &ctx);
    }
    store(&ctx.st.all_created, true, release);

    sleep(1);
    store(&ctx.st.stop, true, release);

    for (int i = 0; i < N_TASKS; ++i)
        pthread_join(th[i], NULL);

    print_ctx(&ctx);
    ctx_cleanup(&ctx);
    return 0;
}
