#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>

#define TOTAL_THREADS 20
#define NUM_REDUCER 8
#define HASH_BIT 8
#define NUM_FILES 15
#define REDUCER_QUEUE_SIZE 1000000
int reducer_stop_flag=0;

struct word{
	char *key;//defines the actual word (which is supposed to be of maximum 50 letters)
	unsigned int hash;//hash code for that word
	int reducer_id;//reducer id
};
struct word word_hash[NUM_REDUCER][REDUCER_QUEUE_SIZE];
int reducer_word_count[NUM_REDUCER];


unsigned count_words(char *str){
	unsigned count = 0;
	int flag = 0;//1 means the point is inside a word, 0 means outside a word
	while(*str){
		if (*str == ' ' || *str == '\n' || *str == '\t' || *str == ','){
			flag = 0;
		}
		else if (flag==0){
			flag = 1;
			++count;
		}
		++str;
	}
	return count;
}
int extract_hash_encoding(char *str){
	int word_length = strlen(str);
	int hash_number;
	int i;

	hash_number = str[0];
	for (i=1; i<word_length; i++){
		//binary = decimal_to_binary(str[i]);
		hash_number ^= str[i];
	}
	return hash_number;
}
void print_hash_codes(char *key,int hash, int reducer_id){
	//print the results in a specific format in the IO
	int te;
	int k;
	printf("%s",key);
	for (k=0; k<abs((20-strlen(key))); k++){
		printf("-");
	}
	printf("H=%d",hash);
	if (hash < 10){
		te = 1;
	}
	else if (hash < 100){
		te = 2;
	}
	else if (hash < 1000){
		te = 3;
	}
	for (k=0; k<abs(10-te); k++){
		printf("-");
	}
	printf("R=%d\n",reducer_id);
	return;
}
int extract_word_from_sentence(char *str, int verbose){
	char delim[] = " ,\t\r;";
	char *p;
	p = (char*)malloc(100 * sizeof(char));
	p = strtok(str,delim);
	int hash_number;
	int actual_number = 0;
	int reducer_id;

	while(p != NULL){
		hash_number = extract_hash_encoding(p);
		if (hash_number != 10){//removing Data Link Escape
			reducer_id = hash_number % NUM_REDUCER;
			if (verbose==1){
				print_hash_codes(p,hash_number,reducer_id);
			}
			actual_number += 1;
		}
		p = (char*)malloc(100 * sizeof(char));
		p = strtok(NULL,delim);
	}
	return actual_number;
}
void reducer_thread(int F,int thread_id, int *mapper_stop_flag, int i){
	F = F-1;//thread id start from 0, but in our code, it starts from 1. So, to adjust, subtract 1
	thread_id = omp_get_thread_num();
	printf("Reducer ID =  %d,Thread ID = %d\n",F, thread_id);
	while(1){
		//Reduce the data
		if (reducer_stop_flag==1){
			break;
		}
	}
	if (F==1){
		for (i=0;i<reducer_word_count[F];i++){
			if (strlen(word_hash[F][i].key) > 0) {
				printf("%s\n",word_hash[F][i].key);
			}

		}
	}

	printf("Exiting Reducer, Thread ID = %d\n",thread_id);
	return;
}

void read_file(int F, FILE *fp, int thread_id,unsigned int word_count, int actual_number, char *file_name, char *file_id, char *file_ext, char *file_dir, char *p, char *buff, int verbose, char *delim,int reducer_id,int hash_number, int i, int tt, char *temp,int word_flag, int *mapper_stop_flag, struct word word_hash[][REDUCER_QUEUE_SIZE],int *reducer_word_count){
	thread_id = omp_get_thread_num();
	word_count=0;
	actual_number=0;
	verbose = 0;

	strcpy(file_dir,"../files/");
	strcpy(file_ext,".txt");
	sprintf(file_id,"%d",F);
	strcpy(file_name,strcat(file_id,file_ext));
	strcpy(file_name,strcat(file_dir,file_name));

	fp = fopen(file_name,"r");
	if (fp == NULL){
		printf("Error in opening file, Thread = %d, File = %s\n",thread_id,file_name);
		return;
	}

	buff = (char*)malloc(100 * sizeof(char));
	while((fgets(buff, 100, (FILE*)fp)) != NULL){
		p = (char*)malloc(100 * sizeof(char));
		//copy the buffer content to a dynmamic array
		strcpy(p,buff);
		//Find the words in the sentence, which is in p and generate their hash codes and reducer id
		while(*p){
			if (*p == ' ' || *p == '\n' || *p == '\t' || *p == ','){
				word_flag=1;
				tt=0;
			}
			else{
				temp[tt++] = p[0];
				word_flag = 0;
			}
			if (word_flag==1){

				hash_number = temp[0];
				for (i=1; i<tt; i++){
					//binary = decimal_to_binary(str[i]);
					hash_number ^= temp[i];
				}
				if (hash_number<0){
					hash_number *= -1;
				}
				if (hash_number != 10){//removing Data Link Escape
					reducer_id = hash_number % NUM_REDUCER;
					//put the data in the reducer queues
					//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					//printf("Count = %d\n",word_count);
					//printf("%s,%d,%d\n",temp,hash_number,reducer_id);
					strcpy(word_hash[reducer_id][reducer_word_count[reducer_id]++].key,temp);
					word_hash[reducer_id][reducer_word_count[reducer_id]++].hash = hash_number;
					word_hash[reducer_id][reducer_word_count[reducer_id]++].reducer_id = reducer_id;
					//printf("%s,%d,%d\n",word_hash[reducer_id][word_count].key,word_hash[reducer_id][word_count].hash,word_hash[reducer_id][word_count].reducer_id);
					//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					if (verbose==1){
						print_hash_codes(temp,hash_number,reducer_id);
					}
					word_count += 1;
				}
				tt=0;
				memset(temp,0,sizeof(temp));
			}
			++p;
		}
	}
	printf("Thread_ID=%d, Total Words = %d, File Name = %s\n",thread_id,word_count,file_name);
	fclose(fp);
	mapper_stop_flag[F-1] = 1;
	return;
}
void stop_criterion_check(int thread_id,int temp_count, int *mapper_stop_flag, int i){
	thread_id = omp_get_thread_num();
	printf("Entered Thread stop criterion function, thread id=%d\n",thread_id);
	while(1){
		temp_count=0;
		for (i=0;i<NUM_FILES;i++){
			if (mapper_stop_flag[i]==0){
				reducer_stop_flag = 0;
				break;
			}
			temp_count++;
		}
		if (temp_count==15){
			reducer_stop_flag=1;
			break;
		}
	}
	printf("Exiting Thread stop criterion function\n");
	return;
}
int main(int argc, char *argv[]){
	int mapper_stop_flag[NUM_FILES];
	int Front=-1;//reducer queue variable
	int Rear=-1;////reducer queue variable
	int F;
	int i,k;
	char temp[100];
	double start_s,start_p,end_s,end_p;
	unsigned int word_count=0;
	FILE *fp;
	int thread_id;
	int actual_number=0;
	int verbose=0;//whether to show the hashcodes
	char file_name[100];
	char file_id[100];
	char file_ext[100];
	char file_dir[100];
	char delim[] = " ,\t\r;";
	char *p;
	char *buff;
	int reducer_id;
	int hash_number;
	int tt=0;
	int word_flag=0;
	int R;
	int temp_count=0;
	buff = (char*)malloc(100 * sizeof(char));

	for (i=0;i<NUM_FILES;i++){
		mapper_stop_flag[i] = 0;
	}
	for (i=0;i<NUM_REDUCER;i++){
		reducer_word_count[i] = 0;
		for (k=0;k<REDUCER_QUEUE_SIZE;k++){
			word_hash[i][k].key = malloc(100*sizeof(char));
			word_hash[i][k].hash = 0;
			word_hash[i][k].reducer_id = 0;
		}
	}
	printf("Allocation Done\n");

	/*
	// Serial code
	printf("\n\n\n");
	start_s = omp_get_wtime();
	for (F=1;F<=NUM_FILES;F++){
		read_file(F,fp,thread_id,word_count,actual_number,file_name,file_id,file_ext,file_dir,p,buff,verbose,delim,reducer_id,hash_number,i,tt,temp,word_flag,mapper_stop_flag,word_hash,reducer_word_count);
	}
	end_s = omp_get_wtime();
	printf("\n\n\n");
	*/

	//Reader & Mapper Threads that generate a hash code
	start_p = omp_get_wtime();
	#pragma omp parallel private(F,fp,thread_id,word_count,actual_number,file_name,file_id,file_ext,file_dir,p,buff,verbose,delim,reducer_id,hash_number,i,tt,temp,word_flag) shared(mapper_stop_flag,reducer_stop_flag,word_hash,reducer_word_count)
	{
		#pragma omp single nowait
		{
			#pragma omp task private(fp,thread_id,word_count,actual_number,file_name,file_id,file_ext,file_dir,p,buff,verbose,delim,reducer_id,hash_number,i,tt,temp,word_flag) shared(mapper_stop_flag,reducer_stop_flag,word_hash,reducer_word_count)
			for (F=1;F<=TOTAL_THREADS;F++){
				//Spawn the reducer threads first
				if (F<=NUM_REDUCER){
					#pragma omp task untied
					reducer_thread(F,thread_id,mapper_stop_flag,i);
					//The reducer threads run indefinitely, takes data from the queues and keep on counting. They only stop once the shared stop flag is set by the reader/mapper thread
					// and there is no element left in the corresponding queue.
				}
				//Spawn the reader threads
				if (F<=NUM_FILES){
					#pragma omp task untied
					read_file(F,fp,thread_id,word_count,actual_number,file_name,file_id,file_ext,file_dir,p,buff,verbose,delim,reducer_id,hash_number,i,tt,temp,word_flag,mapper_stop_flag,word_hash,reducer_word_count);
				}
				//Stop criterion thread; used to stop the reducer threads
				if (F==TOTAL_THREADS){
					#pragma omp task private(thread_id,i)
					stop_criterion_check(thread_id,temp_count,mapper_stop_flag,i);
				}
			}
		}
	}
	#pragma omp barrier
	end_p = omp_get_wtime();

	printf("\n\n\n");
	printf("Reducing Complete\n");
	printf("Time for Sequential Run = %f s\n",end_s-start_s);
	printf("Time for Parallel Run = %f s\n",end_p-start_p);
	return 0;
}
