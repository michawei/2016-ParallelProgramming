#include <stdio.h>
#include <cuda.h>

int main() {
	/* Get Device Num */
	int cudaDeviceNum = 0;
	cudaGetDeviceCount(&cudaDeviceNum);
	printf("%d devices found supporting CUDA\n", cudaDeviceNum);

	if ( cudaDeviceNum == 0 ) {
		printf("No GPU\n");
		return 0;
	}

	for (int i = 0; i < cudaDeviceNum; i++) {
		cudaDeviceProp deviceInfo;
		cudaGetDeviceProperties(&deviceInfo, i);
		printf("----------------------------------\n");
		printf("Device %s\n", deviceInfo.name);
		printf("----------------------------------\n");
		printf(" Device memory: \t%zu\n", deviceInfo.totalGlobalMem);
		printf(" Memory per-block: \t%zu\n", deviceInfo.sharedMemPerBlock);
		printf(" Register per-block: \t%d\n", deviceInfo.regsPerBlock);
		printf(" Warp size: \t\t%d\n", deviceInfo.warpSize);
		printf(" Memory pitch: \t\t%zu\n", deviceInfo.memPitch);
		printf(" Constant Memory: \t%zu\n", deviceInfo.totalConstMem);
		printf(" Max thread per-block: \t%d\n", deviceInfo.maxThreadsPerBlock);
		printf(" Max thread dim: \t%d / %d / %d\n", deviceInfo.maxThreadsDim[0], deviceInfo.maxThreadsDim[1], deviceInfo.maxThreadsDim[2]);
		printf(" Max grid size: \t%d / %d / %d\n", deviceInfo.maxGridSize[0], deviceInfo.maxGridSize[1], deviceInfo.maxGridSize[2]);
		printf(" Ver: \t\t\t%d.%d\n", deviceInfo.major, deviceInfo.minor);
		printf(" Clock: \t\t%d\n", deviceInfo.clockRate);
		printf(" Texture Alignment: \t%zu\n", deviceInfo.textureAlignment);
	}

	return 0;
}