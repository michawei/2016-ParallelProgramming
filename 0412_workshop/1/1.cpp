#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h> /* for sleep */
#include <iostream>
#include <queue>
#include <time.h>

using namespace std;

class Queue{
	private:
		int size;
		int front,rear;
		int *buffer;
	public:
		Queue(int s){
			size=s;
			buffer=new int[s];
			front=0;
			rear=0;
		}
		void enque(int item) {
			if(full()!=true) {
				buffer[front]=item;
				front=(front+1)%size;
			}
			// else
			// 	cout<<"Queue is full!"<<endl;
		}
		int deque(){
			if(empty()!=true){
				int item;
				item=buffer[rear];
				rear=(rear+1)%size;
				return item;
			}
			// cout<<"Queue is empty!"<<endl;
		}
		int now_size(){
			if (front > rear)
				return (front - rear);
			else if ( front == rear )
				return 0;
			else
				return size + front - rear;
		}
		int full(){
			return (((front+1)%size)==rear);
		}
		int empty(){
			return (rear==front);
		}
};

int main(int argc, char *argv[]) {
	// assert(argc == 3);
	// omp_set_num_threads(atoi(argv[1]));
	// int n = atoi(argv[2]);
	// printf("# of proc = %d\n", omp_get_num_procs());
	// printf("# of loop iterations = %d\n", n);
	// int elapsedTime = 0;
	Queue works(16);
#pragma omp parallel sections
	{
#pragma omp section
		{
			srand (time(NULL));
			while (1) {
				if ( works.now_size() < 15 ) {
					int work = rand() % 10 + 1;
					cout << "Producer gives " << work << " second " << endl;
					works.enque(work);
				}
			}
		}
#pragma omp section
		{
			while (1) {
				if ( works.now_size() > 5 ) {
					int work = works.deque();
					cout << "Consumer sleeps " << work << " second " << endl;
					sleep(work);
				}
			}
		}
	}
	return 0;
}