#include <stdio.h>
#include <pthread.h>
 
//#define DEBUG 1
#define MAX_THREAD 8
#define UINT unsigned long
#define MAXN 2048
void rand_gen(UINT c, int N, UINT A[][MAXN]) {
    UINT x = 2, n = N*N;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            x = (x * x + c + i + j)%n;
            A[i][j] = x;
        }
    }
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
int NumberOfRow;
UINT A[MAXN][MAXN], B[MAXN][MAXN], C[MAXN][MAXN];
pthread_mutex_t numSolutionLock;

typedef struct multiData {
    int topIndex;
    int bottomIndex;
} MultiData;

void* multiply(void *multidata) {
    MultiData data = *((MultiData *)multidata);
    for (int i = data.topIndex; i <= data.bottomIndex; i++) {
        for (int j = 0; j < NumberOfRow; j++) {
            unsigned long sum = 0;    // overflow, let it go.
            for (int k = 0; k < NumberOfRow; k++)
                sum += A[i][k] * B[k][j];
            C[i][j] = sum;
        }
    }
    free(multidata);
    pthread_exit(NULL);
}

int MIN(int a, int b) {
    return (a < b) ? a : b;
}

int main() {
    int N, S1, S2;
    while (scanf("%d %d %d", &N, &S1, &S2) == 3) {
        NumberOfRow = N;
        rand_gen(S1, N, A);
        rand_gen(S2, N, B);
        int BLOCK = (N + (MAX_THREAD-1)) / MAX_THREAD;
        int count_thread = 0;

        pthread_t threadList[MAX_THREAD];

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        pthread_mutex_init(&numSolutionLock, NULL);

        for (int i = 0; i < N; ) {
            MultiData *multidata = (MultiData*) malloc(sizeof(MultiData));
            multidata -> topIndex = i;
            multidata -> bottomIndex = MIN(i + BLOCK - 1, N);
            i += BLOCK;
            pthread_create(&threadList[count_thread], &attr, multiply, multidata);
            count_thread++;
        }

        for (int i = 0; i<count_thread; i++ ) {
            pthread_join(threadList[i], NULL);
        }
        //multiply(N, A, B, C);
#ifdef DEBUG
        print_matrix(N, A);
        printf("\n");
        print_matrix(N, B);
        printf("\n");
        print_matrix(N, C);
        printf("\n");
#endif
        printf("%u\n", signature(N, C));
    }
    pthread_exit(NULL);
    return 0;
}
