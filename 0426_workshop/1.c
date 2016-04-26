#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 10000000

int main() {
	//omp_set_num_threads(atoi(argv[1]));
	double t = omp_get_wtime();
	int A[N];
	int B[N];
#pragma omp parallel
	{
#pragma omp for
	for (int i = 0; i < N; i++) {
		A[i] = 5;
	}
#pragma omp for
	for (int j = 0; j < N; j++){
		B[j] = 8;
	}
	}
	printf("time = %f\n", omp_get_wtime() - t);
}