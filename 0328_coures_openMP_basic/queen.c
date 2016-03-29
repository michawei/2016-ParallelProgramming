#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define MAXN 20
int n; /* a global n */

int ok(int position[], int next, int test) {
	for (int i = 0; i < next; i++)
		if (position[i] == test || (abs(test - position[i]) == next - i))
			return 0;
	return 1;
}

int queen(int position[], int next) {
	if (next >= n)
		return 1;
	int sum = 0;
	for (int test = 0; test < n; test++)
		if (ok(position, next, test)) {
			position[next] = test;
			sum += queen(position , next + 1);
		}
	return sum;
}

int main (int argc, char *argv[]) {
	assert(argc == 2);
	n = atoi(argv[1]);
	assert(n <= MAXN);
	int position[MAXN];
	int numSolution = 0;
#pragma omp parallel for private (position) reduction(+:numSolution)
	for (int i = 0; i < n; i++) {
		position [0] = i;
		int num = queen(position , 1);
		printf("iteration %d # of solution = %d\n", i, num);
		numSolution += num;
	}
	printf("total # of solutions = %d\n", numSolution);	
	return 0;
}
