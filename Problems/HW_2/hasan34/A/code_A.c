#include <stdio.h>
#include <omp.h>
#include <unistd.h>
int main()
{
	int len=30;
	char name[30];
	double start, end;
	int thread_id, num_threads;
	start = omp_get_wtime();
	/* Master thread obtains information about itself and its environment. */
	thread_id = omp_get_thread_num();
	num_threads = omp_get_num_threads();
	gethostname(name,len);

	printf("Serial Region (Master Thread):	Runhost:%s	Thread:%d/%d \n",name,thread_id,num_threads);

	/* Parallel Region */
	#pragma omp parallel private(thread_id,num_threads,name,start,end)
	/*Section Pragma*/
	{
		start = omp_get_wtime();
		thread_id = omp_get_thread_num();
		num_threads = omp_get_num_threads();
		gethostname(name,len);
		end = omp_get_wtime();
		printf("Parallel Region:	Runhost:%s	Thread:%d/%d	Time:%f\n",name,thread_id,num_threads,end-start);
		#pragma omp single
		printf("finishing work");
	}
	/* Close parallel region. */

	end = omp_get_wtime();
	printf("Master Thread took = %f seconds\n",end-start);
	return 0;
}
