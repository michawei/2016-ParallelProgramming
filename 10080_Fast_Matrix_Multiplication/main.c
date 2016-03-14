#include <stdio.h>
#include <pthread.h>
#include "matrix.h"
 
// #define DEBUG
#define UINT unsigned long
#define MAXN 2048
#define THREADNUM 2

void rand_gen(UINT c, int N, UINT A[][MAXN]) {
    UINT x = 2, n = N*N;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            x = (x * x + c + i + j)%n;
            A[i][j] = x;
        }
    }
}

typedef struct matrixData {
    UINT c;
    int N;
    UINT **A;
} MatrixData;

void* thread_rand_gen(void *matrixdata) {
    MatrixData matrix = *((MatrixData*) matrixdata)
    UINT x = 2;
    UINT n = matrix.N * matrix.N;
    UINT **ANS = matrix.A;
    for (int i = 0; i < matrix.N; i++) {
        for (int j = 0; j < matrix.N; j++) {
            x = (x * x + matrix.c + i + j)%n;
            ANS[i][j] = x;
        }
    }
    free(matrix);
}

void print_matrix(int N, UINT A[][MAXN]) {
    for (int i = 0; i < N; i++) {
        fprintf(stderr, "[");
        for (int j = 0; j < N; j++)
            fprintf(stderr, " %u", A[i][j]);
        fprintf(stderr, " ]\n");
    }
}
UINT hash(UINT x) {
    return (x * 2654435761LU);
}
UINT signature(int N, UINT A[][MAXN]) {
    UINT h = 0;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++)
            h = hash(h + A[i][j]);
    }
    return h;
}
UINT A[MAXN][MAXN], B[MAXN][MAXN], C[MAXN][MAXN];
int main() {
    int N, S1, S2;
    pthread_t threadList[THREADNUM];
    while (scanf("%d %d %d", &N, &S1, &S2) == 3) {
        // rand_gen(S1, N, A);
        // rand_gen(S2, N, B);
        MatrixData *matrixdata = (MatrixData*) malloc(sizeof(MatrixData));
        matrixdata -> c = S1;
        matrixdata -> N = N;
        matrixdata -> A = A;
        pthread_create(&threadList[0], NULL, thread_rand_gen, matrixdata);
        matrixdata -> c = S2;
        matrixdata -> A = B;
        pthread_create(&threadList[1], NULL, thread_rand_gen, matrixdata);
        for ( int i = 0 ; i < THREADNUM ; i++ ) {
            pthread_join(threadList[i], NULL);
        }
        multiply(N, A, B, C);
#ifdef DEBUG
        print_matrix(N, A);
        print_matrix(N, B);
        print_matrix(N, C);
#endif
        printf("%u\n", signature(N, C));
    }
    return 0;
}
