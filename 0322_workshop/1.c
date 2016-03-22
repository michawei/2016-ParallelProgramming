#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 8

void *printHello(void *thread_id) {
	int tid = *(( int *) thread_id );
	printf("printHello: information = %d\n", tid);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_t threads;
	int t=2580;
	printf("main: create thread %d\n", 1);
	int rc = pthread_create(&threads, NULL, printHello , (void *)(&t));
	if (rc) {
		printf("main: error code %d\n", rc); exit ( -1);
	} 
	pthread_exit(NULL);
	return 0;
}

