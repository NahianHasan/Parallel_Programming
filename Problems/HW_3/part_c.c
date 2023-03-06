#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define NUMROWS 5
#define NUMCOLS 5

float* matrix_addition(float* A, float* B){
	float* N = (float*) malloc(NUMROWS * NUMCOLS * sizeof(float));
	for (int i=0; i<NUMROWS; i++){
		for (int j=0; j<NUMCOLS; j++){
			N[i*NUMCOLS+j] = A[i*NUMCOLS+j] + B[i*NUMCOLS+j];
		}
	}
	return N;
}

float* matrix_multiplication(float* A, float* B, int factor){
	float* N = (float*) malloc(NUMROWS * NUMCOLS * sizeof(float));
	float sum = 0;
	for (int i=0; i<NUMROWS; i++){
		for (int j=0; j<NUMCOLS; j++){
			sum=0;
			for (int k=0;k<NUMCOLS;k++){
				sum += A[i*NUMCOLS+k]*B[k*NUMCOLS+j];
			}
			N[i*NUMCOLS+j] = sum*factor;
		}
	}
	return N;
}

float* newArray(int rows, int cols) {
	float* a = (float*) malloc(NUMROWS * NUMCOLS * sizeof(float));
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				a[i*NUMCOLS+j] = i+j;
			}
		}
	return a;
}

int main(int argc, char* argv[]){
	float* A = newArray(NUMROWS, NUMCOLS);
	float* B = newArray(NUMROWS, NUMCOLS);
	float* MA;
	float* MM;

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			MA = matrix_multiplication(A,B,1);
		}

		#pragma omp section
		{
			MM = matrix_multiplication(A,B,2);
		}
	}
	printf("Matrix Multiplication Thread 1\n");
	for (int i=0; i<NUMROWS; i++){
		for (int j=0; j<NUMCOLS; j++){
			printf("%f ",MA[i*NUMCOLS+j]);
		}
		printf("\n");
	}
	printf("\n\nMatrix Multiplication Thread 2\n");
	for (int i=0; i<NUMROWS; i++){
		for (int j=0; j<NUMCOLS; j++){
			printf("%f ",MM[i*NUMCOLS+j]);
		}
		printf("\n");
	}
	return 0;

}
