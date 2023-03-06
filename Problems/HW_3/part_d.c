#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	printf("Pragma omp parallel\n");
	#pragma omp parallel
	{
		for (int i=0;i<3;i++){
			printf("TID = %d, Iteration Number = %d\n",omp_get_thread_num(),i);
		}
	}
	printf("Pragma omp parallel for\n");
	#pragma omp parallel for
	for (int i=0;i<3;i++){
		printf("Iteration Number = %d\n",i);
	}
	return 0;
}
