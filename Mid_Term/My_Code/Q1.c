#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

#define PROBLEMSIZE 10000000
#define NUMTHREADS 4


void serial_dotprod(float *a, float*b, int start, int end, double* result, int idx, int arraysize){
	//int thread_id = omp_get_thread_num();
	//printf("Thread Id = %d\n",thread_id);
	double res = 0;
	int i;
	//printf("%d,%d\n",start,end);
	for (i=start; i<=end; i++){
		res += a[i]*b[i];
	}
	result[idx] = res;
}

int main( ) {

	float* a;
	float* b;
	double* result;
	int i;
	double serial_result = 0;
	double parallel_result=0;
	double reduction_result = 0;
	int block_size;
	int start, end;
	a = malloc(sizeof(float)*PROBLEMSIZE);
	b = malloc(sizeof(float)*PROBLEMSIZE);
	result = malloc(sizeof(double)*NUMTHREADS);

	if (a==NULL) {
		printf("a is null\n");
		fflush(stdout);
	}

	if (b==NULL) {
		printf("b is null\n");
		fflush(stdout);
	}
	double execTime;


	for (i=0; i<PROBLEMSIZE; i++) {
		a[i] = b[i] = 1;
	}

	for (i=0; i<NUMTHREADS; i++) {
		result[i] = 0;
	}

	// sequential execution to check the answer
	execTime = -omp_get_wtime( );
	block_size = PROBLEMSIZE/(float)NUMTHREADS + 1;

	start = 0;
	end = start+block_size-1;
	for (i=0; i<NUMTHREADS; i++){
		serial_dotprod(a, b, start, end, result, i, PROBLEMSIZE);
		start = end+1;
		end = start+block_size-1;
		if (end > PROBLEMSIZE) {
			end = PROBLEMSIZE;
		}
	}

	for (i=0; i<NUMTHREADS; i++){
		serial_result += result[i];
	}
	execTime += omp_get_wtime( );
	printf("dot product sequential result: %lf, time taken %lf\n", serial_result, execTime); fflush(stdout);


	for (i=0; i<NUMTHREADS; i++) {
		result[i] = 0;
	}
	// parallel version with tasks
	execTime = -omp_get_wtime( );
	#pragma omp parallel
	{
		#pragma omp single
		{
			start = 0;
			end = start+block_size-1;
			for (i=0; i<NUMTHREADS; i++){
				#pragma omp task untied
					serial_dotprod(a, b, start, end, result, i,PROBLEMSIZE);
				start = end+1;
				end = start+block_size-1;
				if (end > PROBLEMSIZE) {
					end = PROBLEMSIZE;
				}
			}
		}
		#pragma omp taskwait
	}
	for (i=0; i<NUMTHREADS; i++){
		parallel_result += result[i];
	}
	execTime += omp_get_wtime();
	printf("dot product parallel result: %lf, time taken %lf\n", parallel_result, execTime);

	// OMP Reduction version
	execTime = -omp_get_wtime( );
	#pragma omp parallel for reduction(+:reduction_result)
	for (i=0; i<PROBLEMSIZE; i++){
		reduction_result += a[i]*b[i];
	}
	execTime += omp_get_wtime();
	printf("dot product omp reduction result: %lf, time taken %lf\n", reduction_result, execTime);
	return 0;
}
