#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct hybrid_lock {
	pthread_mutex_t h_lock;
	int h_count;
}hybrid_lock;

int hybrid_lock_init(struct hybrid_lock * restrict hybrid, const pthread_mutexattr_t *restrict attr);
int hybrid_lock_destroy(struct hybrid_lock *hybrid);
int hybrid_lock_lock(struct hybrid_lock *hybrid);
int hybrid_lock_unlock(struct hybrid_lock *hybrid);
