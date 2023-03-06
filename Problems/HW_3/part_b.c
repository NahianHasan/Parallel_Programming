#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int tid=1;
	int i;
	#pragma omp parallel private(tid)
	{
		tid = omp_get_thread_num();
		int result;int w=1;
		while (w<=10){
			result = tid*tid;
			w+=1;
			tid = omp_get_thread_num();
		}
		#pragma omp critical
		printf("The address of variable 'result' is = %d\n",result);
	}

	#pragma omp parallel private(tid)
	{
		tid = omp_get_thread_num();
		int result;int w=1;
		while (w<=10){
			result = tid*tid;
			w+=1;
			tid = omp_get_thread_num();
		}
		#pragma omp master
		printf("The address of variable in master 'result' is = %d\n",result);
	}

	#pragma omp parallel private(tid)
	{
		tid = omp_get_thread_num();
		int result;int w=1;
		while (w<=10){
			result = tid*tid;
			w+=1;
			tid = omp_get_thread_num();
		}
		#pragma omp single
		printf("The address of variable in single 'result' is = %d\n",result);
	}
	return 0;
}
