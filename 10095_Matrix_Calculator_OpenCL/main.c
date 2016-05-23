/* header */
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <CL/cl.h>
#include <omp.h>

#define MAXGPU 10
#define MAXK 1024
#define DEVICENUM 2
#define MAXLOG 4096

#define UINT uint32_t
#define MAXN 1024

void add(int N, UINT *A, UINT *B, UINT *C) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            C[i * N + j] = A[i * N + j] + B[i * N + j];
    }
}

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
	clCreateContext(NULL, DEVICENUM, GPU, NULL, NULL, 
			&status);
	//assert(status == CL_SUCCESS);

	/* commandqueue */
	cl_command_queue commandQueue[DEVICENUM];
	for (int device = 0; device < DEVICENUM; device++) {
		commandQueue[device] = 
		clCreateCommandQueue(context, GPU[device],
			CL_QUEUE_PROFILING_ENABLE, 
			&status);
		//assert(status == CL_SUCCESS);
	}

	/* kernelsource */
	FILE *kernelfp = fopen("matrix-lib.cl", "r");
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
	status = clBuildProgram(program, DEVICENUM, GPU, NULL, NULL, NULL);
	//assert(status == CL_SUCCESS);
	if (status != CL_SUCCESS) {
		char log[MAXLOG];
		size_t logLength;
		for (int device = 0; device < DEVICENUM; device++) {
		  	clGetProgramBuildInfo(program, GPU[device], 
			CL_PROGRAM_BUILD_LOG,
			MAXLOG, log, &logLength);
			puts(log);
		}
		exit(-1);
	}
	//printf("%d\n", status);
	//printf("Build program completes\n");

	/* createkernel */
	cl_kernel kernel = clCreateKernel(program, "matrixMul", &status);
	//assert(status == CL_SUCCESS);
	//printf("%d\n", status);
	//printf("Build kernel completes\n");

	int N;
	scanf("%d", &N);
	int S[N];
	for ( int i = 0 ; i < 6 ; i++ ) {
		scanf("%d", &S[i]);
	}

	/* vector */
	uint32_t length = N*N;
	cl_uint* A = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* B = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* C = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* D = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* E = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* F = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* X = (cl_uint*)malloc(length * sizeof(cl_uint));
	cl_uint* Y = (cl_uint*)malloc(length * sizeof(cl_uint));
	//assert(A != NULL && B != NULL && C != NULL);

	rand_gen(S[0], N, A);
	rand_gen(S[1], N, B);
	rand_gen(S[2], N, C);
	rand_gen(S[3], N, D);
	rand_gen(S[4], N, E);
	rand_gen(S[5], N, F);
	//double t = omp_get_wtime ();

	//multiply(N, A, B, C);

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
	cl_mem bufferE = 
	clCreateBuffer(context, 
		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		   length * sizeof(cl_uint), E, &status);
	//assert(status == CL_SUCCESS);
	cl_mem bufferF = 
	clCreateBuffer(context, 
		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		   length * sizeof(cl_uint), F, &status);
	//assert(status == CL_SUCCESS);


	cl_mem bufferX = 
	clCreateBuffer(context, 
		   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		   length * sizeof(cl_uint), X, &status);
	//assert(status == CL_SUCCESS);

	/* createbuffer */
	cl_mem bufferC = 
	clCreateBuffer(context, 
		   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		   length * sizeof(cl_uint), C, &status);
	//assert(status == CL_SUCCESS);
	cl_mem bufferD = 
	clCreateBuffer(context, 
		   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		   length * sizeof(cl_uint), D, &status);
	//assert(status == CL_SUCCESS);
	cl_mem bufferY = 
	clCreateBuffer(context, 
		   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		   length * sizeof(cl_uint), Y, &status);
	//assert(status == CL_SUCCESS);
	//printf("Build buffers completes\n");

	/* ---------------------------------  A * B ---------------------------------- */

	int wN = N;
	/* setarg */
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), 
			  (void*)&bufferA);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), 
			  (void*)&bufferB);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
			  (void*)&bufferX);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 3, sizeof(int), 
			  (void*)&wN);
	//assert(status == CL_SUCCESS);
	//printf("Set kernel arguments completes\n");

	/* setshape */
	size_t globalThreads[] = {(size_t)1024};
	size_t localThreads[] = {256};
	cl_event events[3];

	status = 
	clEnqueueNDRangeKernel(commandQueue[0], kernel, 1, NULL, 
			   globalThreads, localThreads, 
			   0, NULL, &(events[0]));
	//assert(status == CL_SUCCESS);
	//printf("Specify the shape of the domain completes.\n");
	/* getcvector */

	/* ---------------------------------  C * D ---------------------------------- */

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), 
			  (void*)&bufferC);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), 
			  (void*)&bufferD);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
			  (void*)&bufferY);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 3, sizeof(int), 
			  (void*)&wN);
	//assert(status == CL_SUCCESS);
	//printf("Set kernel arguments completes\n");

	status = 
	clEnqueueNDRangeKernel(commandQueue[1], kernel, 1, NULL, 
			   globalThreads, localThreads, 
			   0, NULL, &(events[1]));
	//assert(status == CL_SUCCESS);
	//printf("Specify the shape of the domain completes.\n");

	clEnqueueReadBuffer(commandQueue[0], bufferX, CL_TRUE, 
			  0, length * sizeof(cl_uint), X, 
			  0, NULL, NULL);
	/* getcvector */
	clEnqueueReadBuffer(commandQueue[1], bufferY, CL_TRUE, 
			  0, length * sizeof(cl_uint), Y, 
			  0, NULL, NULL);


	//print_matrix(N, X);
	//print_matrix(N, Y);
	cl_uint* ANS = (cl_uint*)malloc(length * sizeof(cl_uint));
	add(N, X, Y, ANS);
	printf("%u\n", signature(N, ANS));


	/* ---------------------------------  X * E ---------------------------------- */

	/* setarg */
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), 
			  (void*)&bufferX);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), 
			  (void*)&bufferE);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
			  (void*)&bufferC);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 3, sizeof(int), 
			  (void*)&wN);
	//assert(status == CL_SUCCESS);
	//printf("Set kernel arguments completes\n");

	status = 
	clEnqueueNDRangeKernel(commandQueue[0], kernel, 1, NULL, 
			   globalThreads, localThreads, 
			   0, NULL, &(events[0]));
	//assert(status == CL_SUCCESS);
	//printf("Specify the shape of the domain completes.\n");
	/* getcvector */

	/* ---------------------------------  Y * F ---------------------------------- */

	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), 
			  (void*)&bufferY);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), 
			  (void*)&bufferF);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
			  (void*)&bufferD);
	//assert(status == CL_SUCCESS);
	status = clSetKernelArg(kernel, 3, sizeof(int), 
			  (void*)&wN);
	//assert(status == CL_SUCCESS);
	//printf("Set kernel arguments completes\n");

	status = 
	clEnqueueNDRangeKernel(commandQueue[1], kernel, 1, NULL, 
			   globalThreads, localThreads, 
			   0, NULL, &(events[1]));
	//assert(status == CL_SUCCESS);
	//printf("Specify the shape of the domain completes.\n");

	clEnqueueReadBuffer(commandQueue[0], bufferC, CL_TRUE, 
			  0, length * sizeof(cl_uint), C, 
			  0, NULL, NULL);
	/* getcvector */
	clEnqueueReadBuffer(commandQueue[1], bufferD, CL_TRUE, 
			  0, length * sizeof(cl_uint), D, 
			  0, NULL, NULL);


	// print_matrix(N, C);
	// print_matrix(N, D);
	add(N, C, D, ANS);
	printf("%u\n", signature(N, ANS));



	free(A);			/* host memory */
	free(B);
	free(C);
	free(D);
	free(E);
	free(F);
	free(X);
	free(Y);
	clReleaseContext(context);	/* context etcmake */
	for (int device = 0; device < DEVICENUM; device++)
    	clReleaseCommandQueue(commandQueue[device]);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseMemObject(bufferA);	/* buffers */
	clReleaseMemObject(bufferB);
	clReleaseMemObject(bufferC);
	clReleaseMemObject(bufferD);
	clReleaseMemObject(bufferE);
	clReleaseMemObject(bufferF);
	clReleaseMemObject(bufferX);
	clReleaseMemObject(bufferY);
	return 0;
}
/* end */
