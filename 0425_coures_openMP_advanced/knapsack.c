#include <stdio.h> 
#include <stdlib.h> 

#define W 5   		// 重量限制 
#define N 3       	// 物品種類 
#define max(a, b) (a) > (b) ? (a):(b);
//#define MIN 1     // 最小重量 

// struct body { 
//     char name[20]; 
//     int size; 
//     int price; 
// }; 

// typedef struct body object; 

//const int N = 100, W = 100000;
int cost[N+1], weight[N+1];
int c[N + 1][W + 1];
 
int knapsack(int n, int w) {
    if (w < 0) return -1e9;
    if (n == 0) return 0;
 
    // if (c[n][w]) return c[n][w];
 
    // int v = 0;
    // for (int i=0; i<n; ++i) // 每個物品都試試看
    //     v = max(v, knapsack(i, w-weight[i]) + cost[i]);
 
    // return c[n][w] = v;

    for ( int i=1 ; i <= n ; i++){
    	for ( int j=1 ; j <= w ; j++ ) {
    		if ( weight[i] <= j ) {
    			int test = cost[i] + c[i-1][j-weight[i]];
    			if ( test > c[i-1][j] )
    				c[i][j] = test;
    			else
    				c[i][j] = c[i-1][j];
    		}
    		else
    			c[i][j] = c[i-1][j];
    	}
    }

    return 0;
}

int main(void) { 
    cost[1] = 6;
    cost[2] = 10;
    cost[3] = 12;

    weight[1] = 1;
    weight[2] = 2;
    weight[3] = 3;


    knapsack(N, W);

    for ( int i = 0; i <= N ; i++ ){
    	for ( int j = 0; j <= W; j++ ) {
    		printf("%d ", c[i][j]);
    	}
    	printf("\n");
    }

    return 0; 
}  
