#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "utils.h"
 
#define MAXN 10000005
#define BLOCK 500
uint32_t prefix_sum[MAXN];

void output(uint32_t presum[], int n) {
    uint32_t hash = 0;
    for (int i = 1; i <= n; i++)
        hash += presum[i] * i;
    printf("%" PRIu32 "\n", hash);
}

__global__ void PrifixSum_1(uint32_t *prefixSum, uint32_t *preBlockSum, int n) {

    int start = blockIdx.x * BLOCK;
    //printf("sss = %d\n", start);
    int end = start + BLOCK;
    end = (end < n) ? end : n;

    uint32_t sum = 0;
    for ( int i = start ; i < end ; i++ ) {
        sum += prefixSum[i];
        prefixSum[i] = sum;
    }
    preBlockSum[blockIdx.x] = sum;
}

__global__ void PrifixSum_2(uint32_t *prefixSum, uint32_t *preBlockSum, int n) {

    int start = blockIdx.x * BLOCK;
    //printf("sss = %d\n", start);
    int end = start + BLOCK;
    end = (end < n) ? end : n;
    uint32_t add = (blockIdx.x == 0) ? 0 : preBlockSum[blockIdx.x - 1];
    for ( int i = start ; i < end ; i++ ) {
        prefixSum[i] += add;
    }
}

int main() {
    int n;
    uint32_t key;
    while (scanf("%d %" PRIu32, &n, &key) == 2) {

        //printf("---------------------------------\n");

        for ( int i = 0 ; i <= n ; i++ ) {
            prefix_sum[i] = encrypt(i, key);
            //printf("%u ", prefix_sum[i]);
        }
        n += 1;
        //printf("\n");
        // upper-bound int
        int block = (n + (BLOCK - 1)) / BLOCK;
        //printf("%d %d\n", n, block);
        uint32_t pre_block_sum[block];

        cudaSetDevice(2);
        uint32_t *d_prefix_sum;
        uint32_t *d_pre_block_sum;
        int size = n * sizeof(uint32_t);
        int second_size = block * sizeof(uint32_t);

        cudaMalloc((void **) &d_prefix_sum, size);
        cudaMalloc((void **) &d_pre_block_sum, second_size);

        cudaMemcpy(d_prefix_sum, prefix_sum, size, cudaMemcpyHostToDevice);
        PrifixSum_1<<< block, BLOCK >>>(d_prefix_sum, d_pre_block_sum, n);
        //cudaMemcpy(prefix_sum, d_prefix_sum, size, cudaMemcpyDeviceToHost);
        cudaMemcpy(pre_block_sum, d_pre_block_sum, second_size, cudaMemcpyDeviceToHost);

        uint32_t sum = 0;
        for ( int i = 0 ; i < block ; i++ ) {
            sum += pre_block_sum[i];
            pre_block_sum[i] = sum;
        }

        // for ( int i = 0 ; i < n ; i++ ) {
        //     printf("%u ", prefix_sum[i]);
        // }
        // printf("\n");

        // for ( int i = 0 ; i < block ; i++ ) {
        //     printf("%u ", pre_block_sum[i]);
        // }
        // printf("\n***\n");

        cudaMemcpy(d_pre_block_sum, pre_block_sum, second_size, cudaMemcpyHostToDevice);
        PrifixSum_2<<< block, 1 >>>(d_prefix_sum, d_pre_block_sum, n);
        cudaMemcpy(prefix_sum, d_prefix_sum, size, cudaMemcpyDeviceToHost);

        // for ( int i = 0 ; i < block ; i++ ) {
        //     printf("%u ", pre_block_sum[i]);
        // }
        // printf("\n***\n");

        // for ( int i = 0 ; i < n ; i++ ) {
        //     printf("%u ", prefix_sum[i]);
        // }
        // printf("\n");
        output(prefix_sum, n);
        cudaFree(d_prefix_sum);
        cudaFree(d_pre_block_sum);
    }
    return 0;
}
