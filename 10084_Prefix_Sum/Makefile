all:
	gcc -std=c99 -O2 -pthread prefixsum-seq.c secret.c -o prefixsum-seq
	time ./prefixsum-seq < input
clean:
	rm prefixsum-seq
