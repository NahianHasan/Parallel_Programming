#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

void doWork(int t) {
   usleep(t);
}

int* initWork(int n) {
   int i;
   double r;
   int* wA = (int *) malloc(sizeof(int)*n);
   for (i = 0; i < n; i++) {
      wA[i] = rand( )%2*i/(n/10);
   }
   return wA;
}

int main (int argc, char *argv[]) {
   int i;
   int *w = initWork(1000);
   int len=30;
   char name[30];
   int chunk=50;
   double start, end;
   int thread_id, num_threads;
   gethostname(name,len);

   start = omp_get_wtime();
   #pragma omp parallel for schedule(static) private(thread_id)
   for (i = 0; i < 1000; i+=50) {
      thread_id = omp_get_thread_num();
      doWork(1000);
      printf("w[%d] = %d, Thread_ID=%d\n", i, w[i],thread_id);
   }
   end = omp_get_wtime();
   printf("Parallel Region (Static Scheduling):	Runhost:%s Time:%f\n\n\n",name,end-start);

   start = omp_get_wtime();
   #pragma omp parallel for schedule(static,chunk) private(thread_id)
   for (i = 0; i < 1000; i+=50) {
      thread_id = omp_get_thread_num();
      doWork(1000);
      printf("w[%d] = %d, Thread_ID=%d\n", i, w[i],thread_id);
   }
   end = omp_get_wtime();
   printf("Parallel Region (Static(%d) Scheduling):	Runhost:%s Time:%f\n\n\n",chunk,name,end-start);

   start = omp_get_wtime();
   #pragma omp parallel for schedule(dynamic) private(thread_id)
   for (i = 0; i < 1000; i+=50) {
      thread_id = omp_get_thread_num();
      doWork(1000);
      printf("w[%d] = %d, Thread_ID=%d\n", i, w[i],thread_id);
   }
   end = omp_get_wtime();
   printf("Parallel Region (Dynamic Scheduling):	Runhost:%s Time:%f\n\n\n",name,end-start);

   start = omp_get_wtime();
   #pragma omp parallel for schedule(dynamic,chunk) private(thread_id)
   for (i = 0; i < 1000; i+=50) {
      thread_id = omp_get_thread_num();
      doWork(1000);
      printf("w[%d] = %d, Thread_ID=%d\n", i, w[i],thread_id);
   }
   end = omp_get_wtime();
   printf("Parallel Region (Dynamic(%d) Scheduling):	Runhost:%s Time:%f\n\n\n",chunk,name,end-start);

   start = omp_get_wtime();
   #pragma omp parallel for schedule(guided) private(thread_id)
   for (i = 0; i < 1000; i+=50) {
      thread_id = omp_get_thread_num();
      doWork(1000);
      printf("w[%d] = %d, Thread_ID=%d\n", i, w[i],thread_id);
   }
   end = omp_get_wtime();
   printf("Parallel Region (Guided Scheduling):	Runhost:%s Time:%f\n\n\n",name,end-start);
}
		
		
		
		
