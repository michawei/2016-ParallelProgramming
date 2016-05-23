__kernel void
matrixMul(__global int* A, 
		  __global int* B, 
		  __global int* C, 
		  int wN)
{

	int index = get_global_id(0); 

	for ( int y = 0 ; y < wN ; y++ ){
		unsigned int value = 0;
		for (int k = 0; k < wN; k++)
		{
			unsigned int elementA = A[index * wN + k];
			unsigned int elementB = B[k * wN + y];
			value += elementA * elementB;
		}
		
		C[index * wN + y] = value;
	}
}