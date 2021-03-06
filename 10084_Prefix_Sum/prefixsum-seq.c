#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <pthread.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 4
uint32_t prefix_sum[MAXN];

int SUM;
int NUM[MAX_THREAD];
pthread_mutex_t numSolutionLock;

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
    return prefixSum[j-1];
}

void* threadCountAllBlock(void *countdata) {
    CountData blockData = *((CountData*) countdata);
    uint32_t *prefixSum = blockData.prefixSum;
    int j = 0;
    for ( int i = blockData.left_index ; i <= blockData.right_index ; i++ ) {
        // sum += (last block sum)
        pthread_mutex_lock(&numSolutionLock);
        prefixSum[j] += blockData.key;
        pthread_mutex_unlock(&numSolutionLock);
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
            int *num;
            pthread_join(threadList[i], (void **) &num);
            NUM[i] = num;
        }


        uint32_t sum = 0;
        count_thread = 0;
        int index = 0;
        //COUNT ALL BLOCK SUM
        for ( int i = 1 + BLOCK ; i <= n ; ) {
            CountData *countdata = (CountData*) malloc(sizeof(CountData));
            countdata -> left_index = i;
            countdata -> right_index = n;//MIN(i + BLOCK - 1, n);   // choose (max_index n) or (next-block - 1)
            countdata -> prefixSum = prefix_sum + i;
            countdata -> key = NUM[count_thread];
            
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
