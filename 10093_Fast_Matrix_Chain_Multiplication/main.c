#include <stdio.h>
#include <stdint.h>

#define MAXN 100
#define MAX 9223372036854775807

int Z[MAXN+1];
int seed[MAXN];
long long int mul[MAXN+1][MAXN+1];
int s[MAXN+1][MAXN+1];

// generate matrix, row-major
uint32_t* rand_gen(uint32_t seed, int R, int C) {
    uint32_t *m = (uint32_t *) malloc(sizeof(uint32_t) * R*C);
    uint32_t x = 2, n = R*C;
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            x = (x * x + seed + i + j)%n;
            m[i*C + j] = x;
            //printf("%u ", x);
        }
        //printf("\n");
    }
    //printf("\n");
    return m;
}

uint32_t hash(uint32_t x) {
    return (x * 2654435761LU);
}

// output
uint32_t signature(uint32_t *A, int r, int c) {
    uint32_t h = 0;
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < c; j++)
            h = hash(h + A[i*c + j]);
    }
    return h;
}

uint32_t* multiply(uint32_t *A, uint32_t *B, int x, int k, int y) {
    //printf("%d %d %d\n", x, k, y);
    uint32_t *m = (uint32_t *) malloc(sizeof(uint32_t) * x*y);

    uint32_t *BT = (uint32_t *) malloc(sizeof(uint32_t) * k*y);

    for (int i = 0; i < k; i++) {
        for (int j = 0; j < y; j++) {
            BT[j * k + i] = B[i * y + j];
        }
    }
    free(B);

#pragma omp parallel for
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            uint32_t sum = 0;    // overflow, let it go.
            int a = i * k;
            int b = j * k;

            for (int t = 0; t < k; t++){
                sum += A[a + t] * BT[b + t];
            }
            m[i * y + j] = sum;
        }
    }
    free(A);
    free(BT);

    return m;
}

uint32_t* compute_cut(int start, int end) {
    //printf("%u %u\n", start, end);

    if ( end == start ) {
        return rand_gen(seed[end-1], Z[end-1], Z[end]);
    }

    if ( end - start == 1 ) {
        return multiply(rand_gen(seed[start-1], Z[start-1], Z[start]),
                        rand_gen(seed[end-1], Z[end-1], Z[end]), Z[start-1], Z[start], Z[end]);
    }

    int cut = s[start][end];
    return multiply(compute_cut(start, cut), compute_cut(cut+1, end), Z[start-1], Z[cut], Z[end]);
}

int main() {
    int N;
    while (scanf("%d", &N) == 1) {

        for ( int i=0 ; i < N+1 ; i++ )
            scanf("%d", &Z[i]);

        for ( int i=0 ; i < N ; i++ )
            scanf("%d", &seed[i]);

        for ( int i=0 ; i <= N ; i++ ) {
            for ( int j=0 ; j <= N ; j++ ){
                mul[i][j] = 0;
                s[i][j] = 0;
            }
        }

        for ( int length = 2 ; length <= N ; length++ ) {
            for ( int i = 1 ; i <= N - length + 1 ; i++ ) {

                int j = i + length - 1;
                mul[i][j] = MAX;

                for ( int k = i ; k <= j-1 ; k++ ) {
                    long long int q = mul[i][k] + mul[k+1][j] + Z[i-1] * Z[k] * Z[j];
                    if ( q < mul[i][j] ) {
                        mul[i][j] = q;
                        s[i][j] = k;
                    }
                }
            }
        }

        uint32_t *ans = compute_cut(1, N);

        // for ( int i=0 ; i<Z[0] ; i++ ){
        //     for ( int j=0 ; j<Z[N] ; j++ ){
        //         printf("%d ", ans[i*Z[N] + j]);
        //     }
        //     printf("\n");
        // }
        printf("%u\n", signature(ans, Z[0], Z[N]));
    }
    return 0;
}

