#include <stdio.h>

/* experiment with N */
/* how large can it be? */
//#define N (2048*2048)
#define N 10240
#define THREADS_PER_BLOCK 4

__global__ void add(int *a, int *b, int *c)
{
    /* insert code to calculate the index properly using blockIdx.x, blockDim.x, threadIdx.x */
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	//printf("%d\n", index);
	if ( index < N ){
		for ( int i = 0 ; i < index ; i++ )
			a[index] += i;

		//__syncthreads();
		//c[index] = a[N-1-index];
	}
}

__global__ void add_2(int *a, int *b, int *c)
{
    /* insert code to calculate the index properly using blockIdx.x, blockDim.x, threadIdx.x */
	int index = blockIdx.x * blockDim.x + threadIdx.x;
	c[index] = a[N-1-index];
}


int main()
{
    int *a, *b, *c;
	int *d_a, *d_b, *d_c;
	int size = N * sizeof( int );

	/* allocate space for device copies of a, b, c */

	cudaMalloc( (void **) &d_a, size );
	cudaMalloc( (void **) &d_b, size );
	cudaMalloc( (void **) &d_c, size );

	/* allocate space for host copies of a, b, c and setup input values */

	a = (int *)malloc( size );
	b = (int *)malloc( size );
	c = (int *)malloc( size );

	for( int i = 0; i < N; i++ ) {
		a[i] = -1;
		b[i] = -1;
		c[i] = -1;
	}

	cudaStream_t streams[2];
	cudaStreamCreate(&streams[0]);
	cudaStreamCreate(&streams[1]);
	/* copy inputs to device */
	/* fix the parameters needed to copy data to the device */
	cudaMemcpy( d_a, a, size, cudaMemcpyHostToDevice );
	cudaMemcpy( d_b, b, size, cudaMemcpyHostToDevice );
	printf("%d %d\n", (N + (THREADS_PER_BLOCK-1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK);
	/* launch the kernel on the GPU */
	/* insert the launch parameters to launch the kernel properly using blocks and threads */
	add<<< (N + (THREADS_PER_BLOCK-1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK , 0, streams[0]>>>( d_a, d_b, d_c );

	cudaStreamSynchronize(streams[0]);
	//cudaMemcpy( a, d_a, size, cudaMemcpyDeviceToHost );

	add_2<<< (N + (THREADS_PER_BLOCK-1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK , 0, streams[1]>>>( d_a, d_b, d_c );

	cudaStreamSynchronize(streams[1]);
	/* copy result back to host */
	/* fix the parameters needed to copy data back to the host */
	cudaMemcpy( c, d_c, size, cudaMemcpyDeviceToHost );

	int sum = 0;
	for ( int i = 0 ; i < N ; i++ ){
		sum += c[i];
	}
	printf( "sum = %d\n", sum);

	// printf( "c[0] = %d\n", 0, c[0]);
	// printf( "c[%d] = %d\n", N-1,c[N-1]);

	/* clean up */

	free(a);
	free(b);
	free(c);
	cudaFree( d_a );
	cudaFree( d_b );
	cudaFree( d_c );
	
	return 0;
} /* end main */
