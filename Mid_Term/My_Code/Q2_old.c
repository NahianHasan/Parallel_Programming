#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define MEMSIZE 100000
#define NUMTHREADS 4

typedef struct node {
   int valIdx;
   int left;
   int right;
} nodeT;

int memIdx = 0;
nodeT* memory;
int data[10];

void initData( ) {
	for (int i=0; i<10; i++) {
		data[i] = 0;
	}
}

void init( ) {
	memIdx = 0;
	memory = (nodeT*) malloc(sizeof(nodeT)*MEMSIZE);
	if (memory == NULL) {
		printf("invalid memory allocation\n");
		fflush(stdout);
	}
	initData( );
}

int myMalloc( ) {
	if (memIdx < MEMSIZE) {
		return memIdx++;
	}
	return -1;
}

int build(int count) {
	int me;
	if ((me = myMalloc( )) < 0) {
		return -1;
	}

	count = ++count % 10;

	memory[me].valIdx = count;
	//printf("%d, %d\n",me,memory[me].valIdx);
	memory[me].left = build(count);
	memory[me].right = build(count);
	return me;
}

void traverse() {
	int me;
	if ((me = myMalloc( )) < 0) {
		return;
	}
	data[memory[me].valIdx] += 1;
	traverse();
	traverse();
}

void traverse_parallel() {
	#pragma omp critical
	memIdx++;
	int thread_ID;
	if (memIdx < MEMSIZE) {
		thread_ID = omp_get_thread_num();
		traverse_parallel();
		//printf("%d, %d\n",thread_ID,count);
		#pragma omp critical
		printf("%d,%d,%d\n",t,thread_ID,memory[memIdx].valIdx);
		data[memory[memIdx].valIdx] += 1;

	}
}

int main( ) {
	int sum=0;
	init( );
	build(-1);
	memIdx = 0;
	//Sequential Traversal
	traverse();
	printf("Data Fields = \n");
	for (int i=0; i<10; i++) {
		printf("%d\n",data[i]);
		sum+=data[i];
	}
	printf("Serial Traversal : Sum of data fields = %d\n",sum);
	//Parallel traversal
	init( );
	memIdx = 0;
	#pragma omp parallel
	{
		#pragma omp master
		{
			#pragma omp task
			traverse_parallel();
		}
		#pragma omp taskwait
	}
	printf("Data Fields = \n");
	sum=0;
	for (int i=0; i<10; i++) {
		printf("%d\n",data[i]);
		sum+=data[i];
	}
	printf("Parallel Traversal : Sum of data fields = %d\n",sum);

}
