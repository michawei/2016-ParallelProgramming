#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <omp.h>

int main () {
  /* initialize random seed: */
	omp_set_num_threads(4);
  /* generate secret number between 1 and 10: */
#pragma omp parallel
	{
		int thread = omp_get_thread_num();
		printf("thread = %d\n", thread);
		srand(thread);
		for ( int i=0 ; i<10 ; i++ ) {
			int secret = rand() % 2147483647;
			if ( thread == 0 )
				printf("%d => %d\n", i, secret);
		} 
	}
  return 0;
}

