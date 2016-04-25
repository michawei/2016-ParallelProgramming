#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h> /* for sleep */
#include <iostream>
#include <string>
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
		int enque(int item) {
			int sucess = -1;
#pragma omp critical
	{
			if(now_size() < 15) {
				buffer[front%size]=item;
				front++;
				//front=(front+1)%size;
				sucess = 1;
			}
			// else
			// 	cout<<"Queue is full!"<<endl;
	}
			return sucess;
		}
		int deque(){
			int item = -1;
#pragma omp critical
	{
			if (now_size() > 5){
				item=buffer[rear%size];
				rear++;
				//rear=(rear+1)%size;
			}
	}
			return item;
			// cout<<"Queue is empty!"<<endl;
		}
		int now_size(){
			// int num;
			// if (front > rear)
			// 	num = (front - rear);
			// else if ( front == rear )
			// 	num = 0;
			// else
			// 	num = size + front - rear;
			// return num;
			return front - rear;
		}
};

int main(int argc, char *argv[]) {
	// assert(argc == 3);
	// omp_set_num_threads(atoi(argv[1]));
	// int n = atoi(argv[2]);
	// printf("# of proc = %d\n", omp_get_num_procs());
	// printf("# of loop iterations = %d\n", n);
	// int elapsedTime = 0;
	Queue works(15);
#pragma omp parallel sections
	{
		// Producer
#pragma omp section
		{
			srand (time(NULL));
			while (1) {
				int work = rand() % 10 + 1;
				int sucess = works.enque(work);
				if ( sucess == -1 )
					continue;
				printf("Producer 0 gives %d second\n", work);
			}
		}
		// Producer
#pragma omp section
		{
			srand (time(NULL));
			while (1) {
				int work = rand() % 10 + 1;
				int sucess = works.enque(work);
				if ( sucess == -1 )
					continue;
				printf("Producer 0 gives %d second\n", work);
			}
		}
		// Consumer
#pragma omp section
		{
			while (1) {
				int work = works.deque();
				if ( work == -1 )
					continue;
				printf("Consumer 0 sleeps %d second\n", work);
				sleep(work);
			}
		}
		// Consumer
#pragma omp section
		{
			while (1) {
				int work = works.deque();
				if ( work == -1 )
					continue;
				printf("Consumer 1 sleeps %d second\n", work);
				sleep(work);
			}
		}
		// Consumer
#pragma omp section
		{
			while (1) {
				int work = works.deque();
				if ( work == -1 )
					continue;
				printf("Consumer 2 sleeps %d second\n", work);
				sleep(work);
			}
		}
	}
	return 0;
}