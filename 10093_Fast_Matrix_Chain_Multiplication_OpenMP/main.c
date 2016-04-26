#include <stdio.h>
#include <stdint.h>
#include "matrix.h"
 
//#define DEBUG
// #define UINT unsigned long
#define MAXN 2048
#define MAX 2147483647
// void rand_gen(UINT c, int N, UINT A[][MAXN]) {
//     UINT x = 2, n = N*N;
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             x = (x * x + c + i + j)%n;
//             A[i][j] = x;
//         }
//     }
// }
// void print_matrix(int N, UINT A[][MAXN]) {
//     for (int i = 0; i < N; i++) {
//         fprintf(stderr, "[");
//         for (int j = 0; j < N; j++)
//             fprintf(stderr, " %u", A[i][j]);
//         fprintf(stderr, " ]\n");
//     }
// }
// UINT hash(UINT x) {
//     return (x * 2654435761LU);
// }
// UINT signature(int N, UINT A[][MAXN]) {
//     UINT h = 0;
//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++)
//             h = hash(h + A[i][j]);
//     }
//     return h;
// }

// generate matrix, row-major
uint32_t* rand_gen(uint32_t seed, int R, int C) {
    uint32_t *m = (uint32_t *) malloc(sizeof(uint32_t) * R*C);
    uint32_t x = 2, n = R*C;
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            x = (x * x + seed + i + j)%n;
            m[i*C + j] = x;
        }
    }
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


int main() {
    int N, S1, S2;
    while (scanf("%d", &N) == 1) {
        int Z[N+1];
        int seed[N];
        for ( int i=0 ; i < N+1 ; i++ )
            scanf("%d", &Z[i]);

        for ( int i=0 ; i < N ; i++ )
            scanf("%d", &seed[i]);

        for ( int i=0 ; i < N+1 ; i++ )
            printf("%d ", Z[i]);

        printf("\n");

        for ( int i=0 ; i < N ; i++ )
            printf("%d ", seed[i]);

        printf("\n");

        uint32_t m[N+1][N+1];
        uint32_t s[N+1][N+1];
        for ( int i=0 ; i <= N ; i++ ) {
            for ( int j=0 ; j <= N ; j++ ){
                m[i][j] = 0;
                printf("%d ", m[i][j]);
                s[i][j] = 0;
            }
            printf("\n");
        }

        for ( int length = 2 ; length <= N ; length++ ) {
            for ( int i = 1 ; i <= N - length + 1 ; i++ ) {

                uint32_t j = i + length - 1;
                printf("%d %d\n",i,j );
                m[i][j] = MAX;

                for ( int k = i ; k <= j-1 ; k++ ) {
                    printf("%d %d %d\n", i, k, j);
                    printf("%d %d\n", m[i][k], m[k+1][j]);
                    printf("%d %d %d\n", Z[i-1], Z[k], Z[j]);
                    uint32_t q = m[i][k] + m[k+1][j] + Z[i-1] * Z[k] * Z[j];
                    printf("%d\n", q);
                    if ( q < m[i][j] ) {
                        m[i][j] = q;
                        s[i][j] = k;
                    }
                }
            }
        }


        for ( int i=0 ; i < N+1 ; i++ ){
            for ( int j=0 ; j < N+1; j++ ) {
                printf("%d ", m[i][j]);
            }
            printf("\n");
        }

        printf("sss\n");

        for ( int i=0 ; i < N+1 ; i++ ){
            for ( int j=0 ; j < N+1; j++ ) {
                printf("%d ", s[i][j]);
            }
            printf("\n");
        }

        // rand_gen(S1, N, A);
        // rand_gen(S2, N, B);
        // multiply(N, A, B, C);
#ifdef DEBUG
        print_matrix(N, A);
        printf("\n");
		print_matrix(N, B);
		printf("\n");
        print_matrix(N, C);
#endif
        //printf("%u\n", signature(N, C));
    }
    return 0;
}
