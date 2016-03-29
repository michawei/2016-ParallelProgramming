#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

typedef struct countData {
    int left_index;
    int right_index;
    int local;
    int thread;
} CountData;

void* threadTask(void *data) {
    CountData blockData = *((CountData*) data);
    int j = 0;
    int localdata = blockData.local;
    int thread = blockData.thread;
    for ( int i = blockData.left_index ; i <= blockData.right_index ; i++ ) {
        localdata += i;
        printf("thread %d runs index %d with v = %d.\n", thread, i, localdata);
    }
    free(data);
    return localdata;
}

int MIN(int a, int b) {
    return (a < b) ? a : b;
}

/* main */
int main(int argc, char *argv[]) {
  assert(argc == 3);
  int MAX_THREAD = atoi(argv[1]);
  pthread_t threadList[MAX_THREAD];
  //omp_set_num_threads(atoi(argv[1]));
  int n = atoi(argv[2]);
  //printf("# of proc = %d\n", omp_get_num_procs());
  printf("# of loop iterations = %d\n", n);
  /* v */
  int v = 101;
  printf("before the loop thread with v = %d.\n", v);

  int BLOCK = (n + (MAX_THREAD-1)) / MAX_THREAD;
  int count_thread = 0;
  for (int i = 0; i < n; i += BLOCK) {
    int local = v;
    CountData *countdata = (CountData*) malloc(sizeof(CountData));
    countdata -> left_index = i;
    countdata -> right_index = MIN(i + BLOCK - 1, n);   // choose (max_index n) or (next-block - 1)
    countdata -> local = local;
    countdata -> thread = count_thread;
    pthread_create(&threadList[count_thread], NULL, threadTask, countdata);
    count_thread ++;
    //v += i;
    //printf("thread %d runs index %d with v = %d.\n", omp_get_thread_num(), i, v);
  }
  int NUM[count_thread];
  for ( int i = 0 ; i < count_thread ; i++ ) {
      int *num;
      pthread_join(threadList[i], (void **) &num);
      NUM[i] = num;
  }

  printf("after the loop thread, with v = %d.\n", NUM[count_thread-1]);
  return 0;
}
/* end */
