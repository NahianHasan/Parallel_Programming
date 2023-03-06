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
	int num_array_elements = 1000000;
	double A[num_array_elements],sum=0;

	/*initialize the array*/
	for(i=0;i<num_array_elements;i++){
		A[i] = i;
	}

	/* Master thread obtains information about itself and its environment. */
	thread_id = omp_get_thread_num();
	num_threads = omp_get_num_threads();
	gethostname(name,len);
	start = omp_get_wtime();
	for(i=0;i<num_array_elements;i++){
		sum += A[i];
	}
	end = omp_get_wtime();
	printf("Serial Summation (Master Thread):	Runhost:%s	Thread:%d/%d	Summation=%f	 Time=%f seconds\n",name,thread_id,num_threads,sum,end-start);

	//Openmp Parallel reduction manually
	start = omp_get_wtime();
	#pragma omp parallel for
	for(i=0;i<num_array_elements;i++){
		res[omp_get_thread_num()*8] += A[i];
	}
	end = omp_get_wtime();
	printf("Parallel Summation:	Runhost:%s	Threads:%d	Summation=%f	 Time=%f seconds\n",name,num_threads,sum,end-start);

	//Openmp parallel reduction method
	start = omp_get_wtime();
	sum=0;
	#pragma omp parallel for reduction(+:sum)
	for(i=0;i<num_array_elements;i++){
		sum += A[i];
	}
	end = omp_get_wtime();
	printf("Parallel Summation (Openmp Reduction):	Runhost:%s	Threads:%d	Summation=%f	 Time=%f seconds\n",name,num_threads,sum,end-start);
	return 0;
}
