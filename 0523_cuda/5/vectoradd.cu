#include <stdio.h>

/* experiment with N */
/* how large can it be? */
#define N 1024
#define THREADS_PER_BLOCK 1024

__global__ void add(int *a, int *b, int *c)
{
    /* insert code to calculate the index properly using blockIdx.x, blockDim.x, threadIdx.x */
	int index = blockIdx.x * blockDim.x + threadIdx.x;

	__shared__ int tmpA[N];
	__shared__ int tmpB[N];

	tmpA[index] = a[index];
	tmpB[index] = b[index];

	__syncthreads();

	for ( int i = 0 ; i < N ; i++ )
		tmpB[index] += tmpA[i];

	//__syncthreads();

	b[index] = tmpB[index];

	__syncthreads();

	// for ( int i = 0 ; i < N ; i++ )
	// 	b[index] += a[i];
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

	for( int i = 0; i < N; i++ )
	{
		a[i] = i;
		b[i] = 0;
		c[i] = 0;
	}

	/* copy inputs to device */
	/* fix the parameters needed to copy data to the device */
	cudaMemcpy( d_a, a, size, cudaMemcpyHostToDevice );
	cudaMemcpy( d_b, b, size, cudaMemcpyHostToDevice );

	/* launch the kernel on the GPU */
	/* insert the launch parameters to launch the kernel properly using blocks and threads */ 
	for ( int i=0 ; i < 10*N ; i++ )
		add<<< 1, THREADS_PER_BLOCK >>>( d_a, d_b, d_c );

	/* copy result back to host */
	/* fix the parameters needed to copy data back to the host */
	cudaMemcpy( b, d_b, size, cudaMemcpyDeviceToHost );


	printf( "b[0] = %d\n",0,b[0] );
	printf( "b[%d] = %d\n",N-2, b[N-2] );
	printf( "b[%d] = %d\n",N-1, b[N-1] );

	/* clean up */

	free(a);
	free(b);
	free(c);
	cudaFree( d_a );
	cudaFree( d_b );
	cudaFree( d_c );
	
	return 0;
} /* end main */
