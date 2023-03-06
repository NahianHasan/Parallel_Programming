#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Q {
   int* q;
   int pos;
   int size;
} Q;

void doWork(int t) {
   usleep(t);
}

struct Q* initQ(int n) {
   int i;
   struct Q *newQ = (struct Q *) malloc(sizeof(Q));
   newQ->q = (int*) malloc(sizeof(int)*n);
   newQ->pos = -1;
   newQ->size = n-1;
   return newQ;
}

void putWork(struct Q* workQ) {
   if (workQ->pos < (workQ->size)) {
      workQ->pos++;
      workQ->q[workQ->pos] = (int) rand( )%2*workQ->pos;
   } else printf("ERROR: attempt to add Q element%d\n", workQ->pos+1);
}

int getWork(struct Q* workQ) {
   if (workQ->pos > -1) {
      int w = workQ->q[workQ->pos];
      workQ->pos--;
      return w;
   } else printf("ERROR: attempt to get work from empty Q%d\n", workQ->pos);
}

int main(int argc, char *argv[]){
  int i;
  int r;
  double start, end;
  int len=30;
  char name[30];
  gethostname(name,len);
  int thread_id;
  int n=100;
  struct Q* A = initQ(n);

  start = omp_get_wtime();
  for(i=0;i<n;i++){
    putWork(A);
  }
  for(i=0;i<n;i++){
    r = getWork(A);
    doWork(1000);
  }
  end = omp_get_wtime();
  printf("\n\nSequential Region:	Runhost:%s     Time:%f\n",name,end-start);

  start = omp_get_wtime();
  #pragma omp parallel for
  for(i=0;i<n;i++){
    #pragma omp critical
    putWork(A);
  }

  #pragma omp parallel private(r)
  {
    #pragma omp critical
    r = getWork(A);
    while (r) {
      doWork(1000);
      r = getWork(A);
    }
  }
  end = omp_get_wtime();
  printf("\n\nParallel Region:	Runhost:%s     Time:%f\n",name,end-start);

  return 0;
}

