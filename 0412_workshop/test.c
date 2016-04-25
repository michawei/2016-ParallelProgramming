#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define N 10

int main(int argc, char *argv[]) {
	int a[N][N], b[N][N];
	omp_set_num_threads(3);
#pragma omp parallel for
	for (int i = 0; i < N; ++i) {
		printf("thread %d init %d\n", omp_get_thread_num(), i);
	}
// #pragma omp parallel
// 	{
// #pragma omp sections
// 		{
// #pragma omp section
// 			{
// 				printf("thread %d init a\n", omp_get_thread_num());
// 				for (int i = 0; i < N; i++)
// 					for (int j = 0; j < N; j++)
// 						a[i][j] = i + j;
// 			}
// #pragma omp section
// 			{
// 				printf("thread %d init b\n", omp_get_thread_num());
// 				for (int i = 0; i < N; i++)
// 					for (int j = 0; j < N; j++)
// 						b[i][j] = i - j;
// 			}
// 		}
// 	}
	return 0;
}