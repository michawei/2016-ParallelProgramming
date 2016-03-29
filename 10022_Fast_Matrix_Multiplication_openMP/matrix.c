#include "matrix.h"
#include "omp.h"
 
void multiply(int N, unsigned long A[][2048], unsigned long B[][2048], unsigned long C[][2048]) {
	int CHUNK = 8;
#ifdef _OPENMP
	omp_set_num_threads(8);
#endif
#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
            unsigned long sum = 0;    // overflow, let it go.
            for (int k = 0; k < N; k++)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    }
}
