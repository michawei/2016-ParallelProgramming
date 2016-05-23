#define N 2048

__kernel void update(__global int GameBoard[2][N][N], __global int *flag, __global int *NN)
{
	int i = get_global_id(0) + 1;
	int j = get_global_id(1) + 1;
	int Flag = *flag;
	int MAX = *NN;
	if ( i <= MAX && j <= MAX ) {
		int neighbor = GameBoard[Flag][i-1][j-1] + GameBoard[Flag][i][j-1] + GameBoard[Flag][i+1][j-1] + GameBoard[Flag][i-1][j] + GameBoard[Flag][i+1][j] + GameBoard[Flag][i-1][j+1] + GameBoard[Flag][i][j+1] + GameBoard[Flag][i+1][j+1];
		GameBoard[!Flag][i][j] = ( (GameBoard[Flag][i][j] == 0 && neighbor == 3) || (GameBoard[Flag][i][j] == 1 && (neighbor == 2 || neighbor == 3)) );
	}
}
