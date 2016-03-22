#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 4
uint32_t prefix_sum[MAXN];

typedef struct countData {
    int left_index;
    int right_index;
    uint32_t *prefixSum;
    uint32_t key;
} CountData;

void* threadCountBlock(void *countdata) {
    CountData blockData = *((CountData*) countdata);
    uint32_t *prefixSum = blockData.prefixSum;
    uint32_t sum = 0;
    int j = 0;
    for ( int i = blockData.left_index ; i <= blockData.right_index ; i++ ) {
        sum += encrypt(i, blockData.key);
        prefixSum[j] = sum;
        j++;
    }
    free(countdata);
}

void* threadCountAllBlock(void *countdata) {
    CountData blockData = *((CountData*) countdata);
    uint32_t *prefixSum = blockData.prefixSum;
    int j = 0;
    for ( int i = blockData.left_index ; i <= blockData.right_index ; i++ ) {
        // sum += (last block sum)
        prefixSum[j] += blockData.key;
        j++;
    }
    free(countdata);
}

int MIN(int a, int b) {
    return (a < b) ? a : b;
}

int main() {
    int n;
    uint32_t key;
    while (scanf("%d %" PRIu32, &n, &key) == 2) {

        // upper-bound int
        int BLOCK = (n + (MAX_THREAD-1)) / MAX_THREAD;
        int count_thread = 0;
        pthread_t threadList[MAX_THREAD];

        // COUNT BLOCK SUM
        for ( int i = 1 ; i <= n ; ) {
            CountData *countdata = (CountData*) malloc(sizeof(CountData));
            countdata -> left_index = i;
            countdata -> right_index = MIN(i + BLOCK - 1, n);   // choose (max_index n) or (next-block - 1)
            countdata -> prefixSum = prefix_sum + i;
            countdata -> key = key;
            i += BLOCK;
            pthread_create(&threadList[count_thread], NULL, threadCountBlock, countdata);
            count_thread++;
        }
        for ( int i = 0 ; i < count_thread ; i++ ) {
            pthread_join(threadList[i], NULL);
        }

        uint32_t sum = 0;
        count_thread = 0;
        // COUNT ALL BLOCK SUM
        for ( int i = 1 ; i <= n ; ) {
            // uint32_t local = sum;
            CountData *countdata = (CountData*) malloc(sizeof(CountData));
            countdata -> left_index = i;
            countdata -> right_index = MIN(i + BLOCK - 1, n);   // choose (max_index n) or (next-block - 1)
            countdata -> prefixSum = prefix_sum + i;
            countdata -> key = sum;
            
            // last block sum
            sum += prefix_sum[MIN(i + BLOCK - 1, n)];
            i += BLOCK;
            pthread_create(&threadList[count_thread], NULL, threadCountAllBlock, countdata);
            count_thread++;
        }
        for ( int i = 0 ; i < count_thread ; i++ ) {
            pthread_join(threadList[i], NULL);
        }
        output(prefix_sum, n);
    }
    return 0;
}

