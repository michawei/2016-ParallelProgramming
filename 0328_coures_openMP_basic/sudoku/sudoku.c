#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int placeNumber(int n, int sudoku[9][9]) {
	if (n == 81)
		return 1;
	int row = n / 9;
	int col = n % 9;
	int blockRow = row / 3;
	int blockCol = col / 3;
	if (sudoku[row][col] != 0)
		return(placeNumber(n + 1, sudoku));
	int numSolution = 0;
	for (int test = 1; test <= 9; test++) {
		int conflict = 0;
		for (int i = 0; i < 9 && !conflict; i++) 
			if (((col != i) && (sudoku[row][i] == test)) || ((row != i) && (sudoku[i][col] == test)))
					conflict = 1;
		if (!conflict) {
			for (int i = 0; i < 3 && !conflict; i++)
				for (int j = 0; j < 3 && !conflict; j++)
					if (sudoku[3 * blockRow + i][3 * blockCol + j] == test)
						conflict = 1;
			if (!conflict) {
				sudoku[row][col] = test;
				numSolution += placeNumber(n + 1, sudoku);
			}
		}
	}
	/* for */
	sudoku[row][col] = 0;
	return numSolution;
}

int main(void) {
	int sudoku [9][9];
	int firstZero = -1;
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++) {
			scanf("%d", &(sudoku[i][j]));
			if (sudoku[i][j] == 0 && firstZero == -1)
				firstZero = i * 9 + j;
		}
#ifdef _OPENMP
	omp_set_num_threads (9);
#endif
	int numSolution = 0;
#pragma omp parallel for reduction(+ : numSolution) firstprivate(sudoku)
	for (int i = 0; i < 9; i++) {
		sudoku[firstZero / 9][firstZero % 9] = i;
		numSolution += placeNumber(firstZero , sudoku);
	}
	printf("# of solution = %d\n", numSolution);
	return 0;
}
