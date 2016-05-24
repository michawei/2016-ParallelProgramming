#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "utils.h"
 
#define MAXN 10000005
#define MAX_THREAD 4
uint32_t prefix_sum[MAXN];

void output(uint32_t presum[], int n) {
    uint32_t hash = 0;
    for (int i = 1; i <= n; i++)
        hash += presum[i] * i;
    printf("%" PRIu32 "\n", hash);
}

int main() {
    int n;
    uint32_t key;
    while (scanf("%d %" PRIu32, &n, &key) == 2) {
        uint32_t sum = 0;
        for (int i = 1; i <= n; i++) {
            sum += encrypt(i, key);
            prefix_sum[i] = sum;
        }
        output(prefix_sum, n);
    }
    return 0;
}
