#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>
#define MAX_N 2048

int N, M;
int GameBoard[2][MAX_N][MAX_N];

int main(int argc, char *argv[]) {

	assert(argc == 4);
	omp_set_num_threads(atoi(argv[1]));

	int policy = atoi(argv[2]);
	int chunk = atoi(argv[3]); omp_set_schedule(policy, chunk);

	scanf("%d %d", &N, &M);
	for (int i = 1; i <= N; i++) {
		char line[N+1];
		scanf("%s", &line);
		for (int j = 1; j <= N; j++) {
			GameBoard[0][i][j] = line[j-1] - '0';
			//printf("%d ", GameBoard[0][i][j] );
		}
		//printf("\n");
	}

	int flag = 0;
	for (int num = 0 ; num < M ; num++ ) {
#pragma omp parallel for schedule(runtime)
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= N; j++) {
				int neighbor = GameBoard[flag][i-1][j-1] + GameBoard[flag][i][j-1] + GameBoard[flag][i+1][j-1] + GameBoard[flag][i-1][j] + GameBoard[flag][i+1][j] + GameBoard[flag][i-1][j+1] + GameBoard[flag][i][j+1] + GameBoard[flag][i+1][j+1];
				if ( (GameBoard[flag][i][j] == 0 && neighbor == 3) || (GameBoard[flag][i][j] == 1 && (neighbor == 2 || neighbor == 3)) )
					GameBoard[!flag][i][j] = 1;
				else
					GameBoard[!flag][i][j] = 0;
			}
		}
		flag = !flag;
	}
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			printf("%d", GameBoard[flag][i][j]);
		}
		printf("\n");
	}

	return 0;
}