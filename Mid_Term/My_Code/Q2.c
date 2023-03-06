#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>
struct node {
	int val;
	int level;
	struct node* l;
	struct node* r;
} nodeT;

#define MAXLEVEL 17
int grand_count=0;
int data[10];
void initData( ) {
	for (int i=0; i<10; i++) {
		data[i] = 0;
	}
}
omp_lock_t lck;

struct node* build(int level,int count) {

	if (level < MAXLEVEL) {
		struct node* p = (struct node*) malloc(sizeof(nodeT));
		p->level = level;
		count = ++count % 10;
		p->val = count;
		//printf("Value=%d  Level=%d\n",p->val,p->level);
		p->l = build(level+1,count);
		p->r = build(level+1,count);

		return p;
	} else {
		return NULL;
	}
}


int traverse(struct node* p, int count) {
	if (p == NULL) return count;
	data[p->val] += 1;
	//printf("Value=%lf   Level=%d\n",p->val,p->level);
	if (p->l == NULL) return count;
	else count = traverse(p->l,count);
	if (p->r == NULL) return count;
	else count = traverse(p->r,count);

	return count;
}


void traverse_parallel(struct node* p) {
	//int thread_id = omp_get_thread_num();
	//printf("Thread Id = %d\n",thread_id);

	//printf("Value=%d   Level=%d\n",p->val,p->level);
	if (p->l) {
		#pragma omp task untied
		{traverse_parallel(p->l);}
	}

	if (p->r) {
		#pragma omp task untied
		{traverse_parallel(p->r);}
	}


	omp_set_lock(&lck);
	data[p->val] += 1;
	omp_unset_lock(&lck);

}

int main( ) {
	int count_s=0;
	int sum=0;
	double threshold = 0.5;
	double start_s,start_p,end_s,end_p,time_1;
	struct node* hp;
	initData( );
	printf("Number of Nodes = %d\n",(int)pow(2,MAXLEVEL)-1);

	struct node* h = build(0,0);
	count_s = traverse(h,count_s);
	printf("Data Fields = \n");
	for (int i=0; i<10; i++) {
		printf("Data[%d] = %d\n",i,data[i]);
		sum+=data[i];
	}
	printf("Serial Traversal : Sum of data fields = %d\n",sum);

	initData( );

	omp_init_lock(&lck);
	#pragma omp parallel
	{
		#pragma omp master
		traverse_parallel(h);
	}
	omp_destroy_lock(&lck);
	sum=0;
	printf("\n\nData Fields = \n");
	for (int i=0; i<10; i++) {
		printf("Data[%d] = %d\n",i,data[i]);
		sum+=data[i];
	}
	printf("Parallel Traversal : Sum of data fields = %d\n",sum);

	printf("The trees are initialized differently than the code provided for experiment purpose. However, both the serial and parallel code provide similar outputs.\n");

}
