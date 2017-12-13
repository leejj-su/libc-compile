#include "hybrid_lock.h"

int hybrid_lock_init(struct hybrid_lock *restrict hybrid, const pthread_mutexattr_t *restrict attr)
{
	return pthread_mutex_init(&hybrid->h_lock, attr);
}

int hybrid_lock_destroy(struct hybrid_lock * hybrid)
{
	return pthread_mutex_destroy(&hybrid->h_lock);
}

int hybrid_lock_lock(struct hybrid_lock *hybrid)
{
	struct timeval  start_t, end_t;
	gettimeofday(&start_t, NULL);

	int chk;
	while(1){
		gettimeofday(&end_t,NULL);
		if((end_t.tv_sec+end_t.tv_usec/1000000.0) - (start_t.tv_sec+start_t.tv_usec/1000000.0)>=1) break;
		chk = pthread_mutex_trylock(&hybrid->h_lock);
		if(chk==0){
			if(hybrid->h_count==0)
 			{
				hybrid->h_count++;
				return chk;
			}
			else{
	
				pthread_mutex_unlock(&hybrid->h_lock);
			}
		}
	}

	hybrid->h_count ++;
	return pthread_mutex_lock(&hybrid->h_lock);
		
}

int hybrid_lock_unlock(struct hybrid_lock *hybrid)
{
	hybrid->h_count--;
	return pthread_mutex_unlock(&hybrid->h_lock);
}
