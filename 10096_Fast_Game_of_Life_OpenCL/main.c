/* header */
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <CL/cl.h>
#include <omp.h>

#define MAXGPU 10
#define MAXK 2048

// #define DEBUG 1
#define MAX_Unit 1024
#define MAXN 2048

cl_uint GameBoard[2][MAXN][MAXN];
/* main */
int main(int argc, char *argv[])
{
	cl_int status;
	cl_platform_id platform_id;
	cl_uint platform_id_got;
	status = clGetPlatformIDs(1, &platform_id, 
				&platform_id_got);
	//printf("%d platform found\n", platform_id_got);

	cl_device_id GPU[MAXGPU];
	cl_uint GPU_id_got;
	status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 
			  MAXGPU, GPU, &GPU_id_got);
	//printf("There are %d GPU devices\n", GPU_id_got); 

	/* getcontext */
	cl_context context = 
	clCreateContext(NULL, 1, GPU, NULL, NULL, 
			&status);

	/* commandqueue */
	cl_command_queue commandQueue =
	clCreateCommandQueue(context, GPU[0], 0, &status);

	/* kernelsource */
	FILE *kernelfp = fopen("game-of-life.cl", "r");
	assert(kernelfp != NULL);
	char kernelBuffer[MAXK];
	const char *constKernelSource = kernelBuffer;
	size_t kernelLength = 
	fread(kernelBuffer, 1, MAXK, kernelfp);
	//printf("The size of kernel source is %zu\n", kernelLength);
	cl_program program =
	clCreateProgramWithSource(context, 1, &constKernelSource, 
				  &kernelLength, &status);

	/* buildprogram */
	status = clBuildProgram(program, 1, GPU, NULL, NULL, NULL);
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
	//printf("Build program completes\n");

	/* createkernel */
	cl_kernel kernel = clCreateKernel(program, "update", &status);
	//printf("Build kernel completes\n");

	int N, M;
	scanf("%d %d", &N, &M);

	/* vector */
	cl_uint *flag = (cl_uint *)malloc(sizeof(cl_uint));
	cl_uint *NN = (cl_uint *)malloc(sizeof(cl_uint));
	//cl_uint *Times = (cl_uint *)malloc(sizeof(cl_uint));
	//cl_uint *unit = (cl_uint *)malloc(sizeof(cl_uint));
	*flag = 0;
	*NN = N;
	//*Times = M;
	//*unit = ceil(sqrt((N * N) / MAX_Unit));
	//printf("%d\n", *unit);

	for (int i = 1; i <= N; i++) {
		char line[N+1];
		scanf("%s", &line);
		for (int j = 1; j <= N; j++) {
			GameBoard[0][i][j] = line[j-1] - '0';
			// printf("%d ", GameBoard[0][i][j] );
		}
		// printf("\n");
	}

	//int Block = (*unit == 0) ? N : 32;
	// *unit = (*unit == 0) ? 1 : *unit;
	//printf("%d\n", Block);
	uint32_t boardSize = 2 * (MAXN) * (MAXN);
	/* createbuffer */
	cl_mem bufferGameBoard = 
	clCreateBuffer(context, 
		   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		   boardSize * sizeof(cl_uint), GameBoard, &status);
	//assert(status == CL_SUCCESS);

	cl_mem bufferFlag = 
	clCreateBuffer(context, 
		   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		   sizeof(cl_uint), flag, &status);
	//assert(status == CL_SUCCESS);

	cl_mem bufferNN = 
	clCreateBuffer(context, 
		   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		   sizeof(cl_uint), NN, &status);
	//assert(status == CL_SUCCESS);


	// /* setarg */
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), 
			  (void*)&bufferGameBoard);
	//assert(status == CL_SUCCESS);

	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), 
			  (void*)&bufferFlag);
	//assert(status == CL_SUCCESS);

	status = clSetKernelArg(kernel, 2, sizeof(cl_mem), 
			  (void*)&bufferNN);
	//assert(status == CL_SUCCESS);


	/* setshape */
	//size_t globalThreads[] = {(size_t)N, (size_t)N};
	size_t globalThreads[] = {(size_t)2048, (size_t)2048};
	size_t localThreads[] = {16, 16};
	for ( int t = 0 ; t < M ; t++ ) {

		status = 
		clEnqueueNDRangeKernel(commandQueue, kernel, 2, NULL, 
				   globalThreads, localThreads, 
				   0, NULL, NULL);

		*flag = !(*flag);
		// printf("%d\n", *flag);

		clEnqueueWriteBuffer(commandQueue, bufferFlag, CL_TRUE, 
				  0, sizeof(cl_uint), flag, 
				  0, NULL, NULL);
		
		// printf("Kernel execution completes.\n");

	}

	clEnqueueReadBuffer(commandQueue, bufferGameBoard, CL_TRUE, 
			  0, boardSize * sizeof(cl_uint), GameBoard, 
			  0, NULL, NULL);
	char c = '0';
	for ( int i = 1 ; i <= N ; i++ ) {
		for ( int j = 1 ; j <= N ; j++ ) {
			//printf("%d", GameBoard[*flag][i][j]);
			//sprintf(&c, "%d", GameBoard[*flag][i][j]);
			putchar(GameBoard[*flag][i][j] + '0');
		}
		putchar('\n');
		//printf("\n");
	}

	free(flag);			/* host memory */
	clReleaseContext(context);	/* context etcmake */
	clReleaseCommandQueue(commandQueue);
	clReleaseProgram(program);
	clReleaseKernel(kernel);
	clReleaseMemObject(bufferGameBoard);	/* buffers */
	clReleaseMemObject(bufferFlag);
	return 0;
}
/* end */
