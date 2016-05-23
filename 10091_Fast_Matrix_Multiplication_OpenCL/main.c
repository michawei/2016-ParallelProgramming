/* header */
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <CL/cl.h>
#include <omp.h>

#define MAXGPU 10
#define MAXK 1024

// #define DEBUG 1
#define UINT uint32_t
#define MAXN 1024
// void multiply(int N, UINT A[][MAXN], UINT B[][MAXN], UINT C[][MAXN]) {
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             UINT sum = 0;    // overflow, let it go.
//             for (int k = 0; k < N; k++)
//                 sum += A[i][k] * B[k][j];
//             C[i][j] = sum;
//         }
//     }
// }
void rand_gen(UINT c, int N, UINT *A) {
	UINT x = 2, n = N*N;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			x = (x * x + c + i + j)%n;
			//A[i][j] = x;
			A[i * N + j] = x;
		}
	}
}
void print_matrix(int N, UINT *A) {
	for (int i = 0; i < N; i++) {
		fprintf(stderr, "[");
		for (int j = 0; j < N; j++)
			fprintf(stderr, " %u", A[i * N + j]);
		fprintf(stderr, " ]\n");
	}
}
UINT signature(int N, UINT *A) {
	UINT h = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
			h = (h + A[i * N + j]) * 2654435761LU;
	}
	return h;
}
//UINT A[MAXN][MAXN], B[MAXN][MAXN], C[MAXN][MAXN];
/* main */
int main(int argc, char *argv[])
{
	//printf("Hello, OpenCL\n");
	cl_int status;
	cl_platform_id platform_id;
	cl_uint platform_id_got;
	status = clGetPlatformIDs(1, &platform_id, 
				&platform_id_got);
	//assert(status == CL_SUCCESS && platform_id_got == 1);
	//printf("%d platform found\n", platform_id_got);

	cl_device_id GPU[MAXGPU];
	cl_uint GPU_id_got;
	status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 
			  MAXGPU, GPU, &GPU_id_got);
	//assert(status == CL_SUCCESS);
	//printf("There are %d GPU devices\n", GPU_id_got); 

	/* getcontext */
	cl_context context = 
	clCreateContext(NULL, 1, GPU, NULL, NULL, 
			&status);
	//assert(status == CL_SUCCESS);

	/* commandqueue */
	cl_command_queue commandQueue =
	clCreateCommandQueue(context, GPU[0], 0, &status);
	//assert(status == CL_SUCCESS);

	/* kernelsource */
	FILE *kernelfp = fopen("matrixmul.cl", "r");
	assert(kernelfp != NULL);
	char kernelBuffer[MAXK];
	const char *constKernelSource = kernelBuffer;
	size_t kernelLength = 
	fread(kernelBuffer, 1, MAXK, kernelfp);
	//printf("The size of kernel source is %zu\n", kernelLength);
	cl_program program =
	clCreateProgramWithSource(context, 1, &constKernelSource, 
				  &kernelLength, &status);
	//assert(status == CL_SUCCESS);

	/* buildprogram */
	status = clBuildProgram(program, 1, GPU, NULL, NULL, NULL);
	//assert(status == CL_SUCCESS);
	if (status == CL_BUILD_PROGRAM_FAILURE) {
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *) malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		printf("%s\n", log);
	}
	//printf("%d\n", status);
	//printf("Build program completes\n");

	/* createkernel */
	cl_kernel kernel = clCreateKernel(program, "matrixMul", &status);
	//assert(status == CL_SUCCESS);
	//printf("%d\n", status);
	//printf("Build kernel completes\n");

	int N;
	uint32_t S1, S2;
	scanf("%d %u %u", &N, &S1, &S2);

	//double t = omp_get_wtime ();

	/* vector */
	uint32_t length = N*N;
	cl_uint* A = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* B = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* C = (cl_uint*)malloc(length * sizeof(cl_uint));
	//assert(A != NULL && B != NULL && C != NULL);


	rand_gen(S1, N, A);
	rand_gen(S2, N, B);
	//multiply(N, A, B, C);

#ifdef DEBUG
	print_matrix(N, A);
	printf("\n");
	print_matrix(N, B);
	//print_matrix(N, C);
#endif
	/* createbuffer */
	cl_mem bufferA = 
	clCreateBuffer(context, 
		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		   length * sizeof(cl_uint), A, &status);
	//assert(status == CL_SUCCESS);
	cl_mem bufferB = 
	clCreateBuffer(context, 
		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		   length * sizeof(cl_uint), B, &status);
	//assert(status == CL_SUCCESS);
	cl_mem bufferC = 
	clCreateBuffer(context, 
		   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
		   length * sizeof(cl_uint), C, &status);
	//assert(status == CL_SUCCESS);
	//printf("Build buffers completes\n");

	int wN = N;
	/* setarg */
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), 
			  (void*)&bufferA);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), 
			  (void*)&bufferB);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
			  (void*)&bufferC);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 3, sizeof(int), 
			  (void*)&wN);
	//assert(status == CL_SUCCESS);
	//printf("Set kernel arguments completes\n");

	/* setshape */
	size_t globalThreads[] = {(size_t)N};
	size_t localThreads[] = {1};
	status = 
	clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, 
			   globalThreads, localThreads, 
			   0, NULL, NULL);
	//assert(status == CL_SUCCESS);
	//printf("Specify the shape of the domain completes.\n");
	/* getcvector */
	cl_int a = clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
			  0, length * sizeof(cl_uint), C, 
			  0, NULL, NULL);
	printf("%d\n", a);
	//printf("Kernel execution completes.\n");
	/* checkandfree */
	// for (int i = 0; i < N; i++){
	// //assert(A[i] + B[i] == C[i]);
	// printf("%d %d %d\n", A[i], B[i], C[i]);
	// }
	// print_matrix(N, C);
	printf("%u\n", signature(N, C));
	//printf("time = %f\n", omp_get_wtime() - t);

	free(A);			/* host memory */
	free(B);
	free(C);
	clReleaseContext(context);	/* context etcmake */
	clReleaseCommandQueue(commandQueue);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseMemObject(bufferA);	/* buffers */
	clReleaseMemObject(bufferB);
	clReleaseMemObject(bufferC);
	return 0;
}
/* end */
