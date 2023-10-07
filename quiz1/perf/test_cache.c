#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "atomic.h"
#include "spinlock.h"

#define REPEATS 1000000
#define N_THREADS 2

struct share {
	atomic int i;
};

static void *task(void *arg)
{
	int i = 0;
	struct share *share = (struct share *) arg;

#ifdef TEST_LD
	while (i++ < REPEATS && 0 == load(&share->i, relaxed))
#else	/* TEST_CAS */
	const int one = 1;
	while (i++ < REPEATS &&
		   0 == compare_exchange_weak(&share->i, &one, 2, relaxed, relaxed))
#endif
		spin_hint();

	return NULL;
}

int main(void)
{
	pthread_t th[N_THREADS];
	struct share share = { .i = 0 };

	for (int i = 0; i < N_THREADS; ++i)
		assert(!pthread_create(&th[i], NULL, task, &share));

	for (int i = 0; i < N_THREADS; ++i)
		assert(!pthread_join(th[i], NULL));

	return 0;
}
