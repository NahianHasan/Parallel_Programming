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
      wA[i] = (int) rand( )%2*i/(n/10);
   }
   return wA;
}
void dowork(int start_ind, int end_int, int *w, int section_id){
  int i;
  for (i = start_ind; i < end_int; i+=10) {
     doWork(1000);
     printf("Section %d, w[%d] = %d,\n", section_id, i, w[i]);
  }
}
int main (int argc, char *argv[]) {
  int i;
  int *w = initWork(1000);
  double start, end;
  int len=30;
  char name[30];
  gethostname(name,len);

  start = omp_get_wtime();
  #pragma omp parallel sections
  {
    #pragma omp section
    {
      dowork(0, 250, w, 1);
    }
    #pragma omp section
    {
      dowork(250, 500, w, 2);
    }
    #pragma omp section
    {
      dowork(500, 750, w, 3);
    }
    #pragma omp section
    {
      dowork(750, 1000, w, 4);
    }
  }
  end = omp_get_wtime();
  printf("\n\nSection Parallel Region (4 Sections):	Runhost:%s     Time:%f\n",name,end-start);

  //Sequential loop
  start = omp_get_wtime();
  for (i = 0; i < 1000; i+=10) {
     doWork(1000);
  }
  end = omp_get_wtime();
  printf("Sequential Run:	Runhost:%s     Time:%f\n\n",name,end-start);
  return 0;
}

