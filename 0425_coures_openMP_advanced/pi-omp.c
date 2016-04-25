#include <omp.h>
#include <stdio.h>
#define N 100000000
#define _OPENMP 1

int main () {
	double x;
	double area = 0.0;
#ifdef _OPENMP
	double t = omp_get_wtime();
#endif

#pragma omp parallel for private(x)
	for (int i = 0; i < N; i++) {
		x = (i + 0.5) / N;
		area += 4.0/(1.0 + x * x);
	}
	double pi = area / N;
#ifdef _OPENMP
	t = omp_get_wtime() - t;
	printf("execution time is %f\n", t);
#endif
	printf("pi = %f\n", pi);
	return 0;
}