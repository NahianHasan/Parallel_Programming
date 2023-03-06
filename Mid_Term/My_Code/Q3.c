#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <math.h>

void initialization(double* current_temperature, int space_points){
	//initialization
	int x;
	for (x=1;x<space_points-1;x++){
		current_temperature[x] = 100;
	}
	current_temperature[0] = 0;
	current_temperature[space_points-1] = 0;
}
int main(){
	double execTime;
	int space_points = 100;
	int time_steps = 1000;
	double* previous_temperature;
	double* current_temperature;
	int t,x;

	current_temperature = malloc(sizeof(double)*space_points);
	previous_temperature = malloc(sizeof(double)*space_points);
	initialization(current_temperature,space_points);
	for (x=0; x<space_points; x++){
		previous_temperature[x] = current_temperature[x];
	}
	//Serial Execution
	//loop through each time step
	execTime = -omp_get_wtime( );
	for (t=0;t<time_steps;t++){
		for (x=1; x<space_points-1; x++){
			current_temperature[x] = (previous_temperature[x-1]+previous_temperature[x]+previous_temperature[x+1])/3.0;
		}
		for (x=0; x<space_points; x++){
			previous_temperature[x] = current_temperature[x];
		}
	}
	execTime += omp_get_wtime( );
	/*
	printf("Serial Execution - time step = %d\n",t);
	for (x=0; x<space_points; x++){
		printf("%lf,",current_temperature[x]);
	}
	printf("\n");
	printf("Rod Temperature - sequential run --- time taken %lf s\n", execTime);
	*/

	initialization(current_temperature,space_points);
	for (x=0; x<space_points; x++){
		previous_temperature[x] = current_temperature[x];
	}
	//Parallel Execution
	//loop through each time step
	execTime = -omp_get_wtime( );
	for (t=0;t<time_steps;t++){
		#pragma omp parallel for
		for (x=1; x<space_points-1; x++){
			current_temperature[x] = (previous_temperature[x-1]+previous_temperature[x]+previous_temperature[x+1])/3.0;
		}
		for (x=0; x<space_points; x++){
			previous_temperature[x] = current_temperature[x];
		}
	}
	execTime += omp_get_wtime( );
	//printf("\n\nParallel Execution - time step = %d\n",t);
	for (x=0; x<space_points; x++){
		printf("%lf ",current_temperature[x]);
	}
	printf("\n");
	//printf("Rod Temperature - Parallel run --- time taken %lf s\n", execTime);
	return 0;
}
