#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
//#include <pthread.h>
#include <omp.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 4
uint32_t prefix_sum[MAXN];

int SUM;
int NUM[MAX_THREAD];

int MIN(int a, int b) {
    return (a < b) ? a : b;
}

int main() {
    int n;
    uint32_t key;
    omp_set_num_threads(MAX_THREAD);
    while (scanf("%d %" PRIu32, &n, &key) == 2) {

        // upper-bound int
        int BLOCK = (n + (MAX_THREAD-1)) / MAX_THREAD;
        //int count_thread = 0;

        // COUNT BLOCK SUM
#pragma omp parallel for
        for ( int i = 1 ; i <= n ; i += BLOCK ) {

            int left_index = i;
            int right_index = MIN(i + BLOCK - 1, n);   // choose (max_index n) or (next-block - 1)
            
            uint32_t sum = 0;
            int j = i;
            for ( int x = left_index ; x <= right_index ; x++ ) {
                sum += encrypt(x, key);
                prefix_sum[j] = sum;
                j++;
            }
            NUM[omp_get_thread_num()] = prefix_sum[j-1];
        }

        //COUNT ALL BLOCK SUM
#pragma omp parallel for
        for ( int i = 1 + BLOCK ; i <= n ; i += BLOCK ) {

            int left_index = i;
            int right_index = n;

            int j = i;
            for ( int x = left_index ; x <=right_index ; x++ ) {
#pragma omp critical
                prefix_sum[j] += NUM[omp_get_thread_num()];
                j++;
            }
        }
        output(prefix_sum, n);
    }
    return 0;
}
