#include <stdio.h>
#include <inttypes.h>
#include "utils.h"
 
#define MAXN 16777216
#define THREADS_PER_BLOCK 512

__global__ void dot(int *A, int *B, int *C) {
	/* insert code to calculate the index properly using blockIdx.x, blockDim.x, threadIdx.x */
	// int index = blockIdx.x * blockDim.x + threadIdx.x;
	__shared__ int tmp[50000];
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	C[idx] = A[idx] * B[idx];
}

int A[MAXN], B[MAXN], C[MAXN];

int main(int argc, char *argv[]) {

	cudaSetDevice(2);
	int *d_A, *d_B, *d_C;
	int size = MAXN * sizeof(int);
	/* allocate space for device copies o f a, b, c */
	cudaMalloc((void **) &d_A, size);
	cudaMalloc((void **) &d_B, size);
	cudaMalloc((void **) &d_C, size);

	int N;
	uint32_t key1, key2;

	while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {
		//int chunk = N / 4;
		for (int i = 0; i < N; i++) {
			A[i] = encrypt(i, key1);
			B[i] = encrypt(i, key2);
		}

		int local_size = N * sizeof(int);

		cudaMemcpy(d_A, A, local_size, cudaMemcpyHostToDevice);
		cudaMemcpy(d_B, B, local_size, cudaMemcpyHostToDevice);
 
		dot<<< (N + (THREADS_PER_BLOCK - 1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK >>>(d_A, d_B, d_C);
		//cudaDeviceSynchronize();
		//dot<<< N, 1>>>(d_A, d_B, d_C);
		//cudaDeviceSynchronize();
		cudaMemcpy(C, d_C, local_size, cudaMemcpyDeviceToHost);

		uint32_t sum = 0;
		for ( int i = 0; i < N; i++ )
			sum += C[i];
		printf("%" PRIu32 "\n", sum);
	}

	// free(A);
	// free(B);
	// free(C);
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);

	return 0;
}