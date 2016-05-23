#include <stdio.h>
//#include <assert.h>
#include <omp.h>
#include <inttypes.h>
#include <CL/cl.h>

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 
 
#define MAXGPU 8
#define MAXK 1024
#define MAXCODESZ 32767
#define MAXN 16777216
static cl_uint A[MAXN], B[MAXN], C[MAXN];
int main(int argc, char *argv[]) {

	/* platform */
	cl_int status;
	cl_platform_id platform_id;
	cl_uint platform_id_got;
	status = clGetPlatformIDs(1, &platform_id, &platform_id_got);
	//assert(status == CL_SUCCESS && platform_id_got == 1);
	//printf("%d platform found\n", platform_id_got);

	cl_device_id GPU[MAXGPU];
	cl_uint GPU_id_got;
	status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, MAXGPU, GPU, &GPU_id_got);
	//assert(status == CL_SUCCESS);
	//printf("There are %d GPU devices\n", GPU_id_got); 

	/* getcontext */
	cl_context context = clCreateContext(NULL, 1, GPU, NULL, NULL, &status);
	//assert(status == CL_SUCCESS);

	/* commandqueue */
	cl_command_queue commandQueue = clCreateCommandQueue(context, GPU[0], 0, &status);
	//assert(status == CL_SUCCESS);

	char name[50];
	scanf("%s", &name);

	/* kernelsource */
	FILE *kernelfp = fopen(name, "r");
	//assert(kernelfp != NULL);
	char kernelBuffer[MAXK];
	const char *constKernelSource = kernelBuffer;
	size_t kernelLength = fread(kernelBuffer, 1, MAXK, kernelfp);
	//printf("The size of kernel source is %zu\n", kernelLength);
	cl_program program = clCreateProgramWithSource(context, 1, &constKernelSource, &kernelLength, &status);
	//assert(status == CL_SUCCESS);

	/* buildprogram */
	status = clBuildProgram(program, 1, GPU, NULL, NULL, NULL);

	//printf("%d\n", status);
	if (status == CL_BUILD_PROGRAM_FAILURE) {
		// Determine the size of the log
		size_t log_size;
		clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

		// Allocate memory for the log
		char *log = (char *) malloc(log_size);

		// Get the log
		clGetProgramBuildInfo(program, GPU[0], CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

		// Print the log
		printf("%s", log);
	}

	// //assert(status == CL_SUCCESS);
	// //printf("Build program completes\n");

	// /* createkernel */
	// cl_kernel kernel = clCreateKernel(program, "MyDot", &status);
	// //assert(status == CL_SUCCESS);
	// //printf("Build kernel completes\n");

	// /* vector */
	// cl_uint* A = (cl_uint*)malloc(MAXN * sizeof(cl_uint));
	// cl_uint* B = (cl_uint*)malloc(MAXN * sizeof(cl_uint));
	// cl_uint* C = (cl_uint*)malloc(MAXN * sizeof(cl_uint));
	// //assert(A != NULL && B != NULL && C != NULL);

	// int N;
	// uint32_t key1, key2;
	// while (scanf("%d %" PRIu32 " %" PRIu32, &N, &key1, &key2) == 3) {
	// 	//int chunk = N / 4;
	// 	for (int i = 0; i < N; i++) {
	// 		A[i] = encrypt(i, key1);
	// 		B[i] = encrypt(i, key2);
	// 	}
	// 	/* createbuffer */
	// 	cl_mem bufferA = 
	// 	clCreateBuffer(context, 
	// 		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
	// 		   N * sizeof(cl_uint), A, &status);
	// 	//assert(status == CL_SUCCESS);
	// 	cl_mem bufferB = 
	// 	clCreateBuffer(context, 
	// 		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
	// 		   N * sizeof(cl_uint), B, &status);
	// 	//assert(status == CL_SUCCESS);
	// 	cl_mem bufferC = 
	// 	clCreateBuffer(context, 
	// 		   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
	// 		   N * sizeof(cl_uint), C, &status);
	// 	//assert(status == CL_SUCCESS);
	// 	//printf("Build buffers completes\n");

	// 	/* setarg */
	// 	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), 
	// 			  (void*)&bufferA);
	// 	//assert(status == CL_SUCCESS);
	// 	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), 
	// 			  (void*)&bufferB);
	// 	//assert(status == CL_SUCCESS);
	// 	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
	// 			  (void*)&bufferC);
	// 	//assert(status == CL_SUCCESS);
	// 	//printf("Set kernel arguments completes\n");
 
	// 	// for (int i = 0; i < N; i++)
	// 	//     C[i] = A[i] * B[i];
 
	// 	/* setshape */
	// 	size_t globalThreads[] = {(size_t)N};
	// 	size_t localThreads[] = {1};
	// 	status = 
	// 	clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, 
	// 			   globalThreads, localThreads, 
	// 			   0, NULL, NULL);
	// 	//assert(status == CL_SUCCESS);
	// 	//printf("Specify the shape of the domain completes.\n");
	// 	/* getcvector */
	// 	clEnqueueReadBuffer(commandQueue, bufferC, CL_TRUE, 
	// 			  0, N * sizeof(cl_uint), C, 
	// 			  0, NULL, NULL);
	// 	//printf("Kernel execution completes.\n");
	// 	/* checkandfree */
	// 	// for (int i = 0; i < N; i++){
	// 	// 	printf("%" PRIu32 " %" PRIu32 " %" PRIu32 "\n", A[i], B[i], C[i]);
	// 	// }

	// 	uint32_t sum = 0;
	// 	for (int i = 0; i < N; i++)
	// 	    sum += C[i];
	// 	printf("%" PRIu32 "\n", sum);

	// 	clReleaseMemObject(bufferA);  /* buffers */
	// 	clReleaseMemObject(bufferB);
	// 	clReleaseMemObject(bufferC);
	// }

	// free(A);           //host memory 
	// free(B);
	// free(C);
	clReleaseContext(context);    /* context etcmake */
	clReleaseCommandQueue(commandQueue);
	clReleaseProgram(program);
	// clReleaseKernel(kernel);

	return 0;
}