#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int main () {
  int iSecret, iGuess;

  /* initialize random seed: */
  srand(0);

  /* generate secret number between 1 and 10: */
	for ( int i=0 ; i<10 ; i++ ) {
		int secret = rand() % 2147483647;
		printf("%d => %d\n", i, secret);
	} 
  iSecret = rand() % 10 + 1;

  return 0;
}
