#include <stdio.h>
#include <omp.h>
#include <unistd.h>
int main()
{
	int len=30;
	char name[30];
	double start, end;
	int thread_id, num_threads,i;
	int nt=omp_get_max_threads();
	int res[nt*8];
	int num_array_elements = 10000000;
	float sum = 0.0;

	/* Master thread obtains information about itself and its environment. */
	thread_id = omp_get_thread_num();
	num_threads = omp_get_num_threads();
	gethostname(name,len);
	start = omp_get_wtime();
	for(i=1;i<=num_array_elements;i++){
		sum += 1.0/i;
	}
	end = omp_get_wtime();
	printf("Serial Summation (Master Thread):	Runhost:%s	Thread:%d/%d	Summation=%f	 Time=%f seconds\n",name,thread_id,num_threads,sum,end-start);
	start = omp_get_wtime();
	sum=0.0;
	for(i=num_array_elements;i>0;i--){
		sum += 1.0/i;
	}
	end = omp_get_wtime();
	printf("Serial Summation (Master Thread):	Runhost:%s	Thread:%d/%d	Summation=%f	 Time=%f seconds\n",name,thread_id,num_threads,sum,end-start);

	//Openmp parallel reduction method
	start = omp_get_wtime();
	sum=0.0;
	#pragma omp parallel for reduction(+:sum)
	for(i=1;i<=num_array_elements;i++){
		sum += 1.0/i;
	}
	end = omp_get_wtime();
	printf("Parallel Summation (Openmp Reduction):	Runhost:%s	Threads:%d	Summation=%f	 Time=%f seconds\n",name,num_threads,sum,end-start);
	return 0;
}
