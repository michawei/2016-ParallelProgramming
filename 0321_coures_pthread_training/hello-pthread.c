#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 8

void *printHello(void *thread_id) {
	int tid = *(( int *) thread_id );
	printf("printHello: tid = %d\n", tid);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t threads[NUM_THREADS];
	int threadIndex[NUM_THREADS];
	for(int t = 0; t < NUM_THREADS; t++) {
		threadIndex[t] = t;
		printf("main: create thread %d\n", t); int rc =
		pthread_create(&threads[t], NULL, printHello , (void *)(threadIndex + t));
		if (rc) {
			printf("main: error code %d\n", rc); exit ( -1);
		} 
	}
	pthread_exit(NULL);
	return 0;
}
