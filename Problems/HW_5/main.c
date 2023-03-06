#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
struct node {
   double val;
   int level;
   struct node* l;
   struct node* r;
} nodeT;

#define MAXLEVEL 18
int grand_count=0;

struct node* build(int level) {

   if (level < MAXLEVEL) {
      struct node* p = (struct node*) malloc(sizeof(nodeT));
      p->level = level;
      p->val = (rand( )%10)/10.0;
      //printf("Value=%lf   Level=%d\n",p->val,p->level);
      p->l = build(level+1);
      p->r = build(level+1);

      return p;
   } else {
      return NULL;
  }
}
struct node* build_parallel(int level) {
	//int thread_id = omp_get_thread_num();
	if (level < MAXLEVEL) {
		//printf("Thread Id = %d\n",thread_id);
		struct node* p = (struct node*) malloc(sizeof(nodeT));
		p->level = level;
		p->val = (rand( )%10)/10.0;
		//printf("Value=%lf   Level=%d\n",p->val,p->level);
		#pragma omp task untied
		p->l = build_parallel(level+1);
		#pragma omp task untied
		p->r = build_parallel(level+1);

		return p;
	} else {
		return NULL;
	}
}

int traverse(struct node* p, int count) {
   if (p == NULL) return count;
   if (p->val < 0.5) {
	count+=1;
   }
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

   //printf("Value=%lf   Level=%d\n",p->val,p->level);
   if (p->l) {
      #pragma omp task untied
      {traverse_parallel(p->l);}
   }

   if (p->r) {
       #pragma omp task untied
       {traverse_parallel(p->r);}
   }

   if (p->val < 0.5) {
      #pragma omp critical
      grand_count+=1;
   }
}

int main( ) {
		omp_set_num_threads(20);
		int count_s=0;
		double threshold = 0.5;
		double start_s,start_p,end_s,end_p,time_1;
		struct node* hp;

		start_s = omp_get_wtime();
		struct node* h = build(0);
		count_s = traverse(h,count_s);
		end_s = omp_get_wtime();

		start_p = omp_get_wtime();

		#pragma omp parallel
		{
			#pragma omp master
			hp = build_parallel(0);
		}
		#pragma omp parallel
		{
			#pragma omp master
			traverse_parallel(hp);
		}
		end_p = omp_get_wtime();

		printf("Tree are separately randomized for sequential and parallel construction. Hence, the number of interest nodes will be different.\n\n");
		printf("Count_sequential = %d\n",count_s);
		printf("Count_parallel = %d\n",grand_count);
		printf("Time for building and traversing Tree (Sequential Run) = %f s\n",end_s-start_s);
		printf("Time for building and traversing Tree (Parallel Run) = %f s\n",end_p-start_p);
		printf("\nComment:\nTraversal slowdowns in the tree might be related to malloc and cache behavior. Malloc requires it to acquire a lock while allocating storage, which has an overhead. Meanwhile, another thread executing malloc will go into a wait queue, and be removed from the queue and executed when the lock is available.  This has some overhead. That's why sometimes the parallel code is slower than the sequential one. I also ran the same code in a multi-thread (16) single processor machine (Intel Core-i-7 12th gen processor). The parallel code runs a little faster than the parallel code in a cluster run. However, still the required time is higher than that of sequential run.\n");
}
