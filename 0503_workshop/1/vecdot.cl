__kernel void MyDot(__global int* A, __global int* B, __global int* C)
{
	int idx = get_global_id(0);’
	C[idx] = A[idx] * B[idx];
}
