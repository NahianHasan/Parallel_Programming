#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>

/////// These variables are only in effect during parallel omp running
#define TOTAL_THREADS 16
#define NUM_REDUCER 8
#define HASH_BIT 8
#define NUM_FILES 15
#define REDUCER_QUEUE_SIZE 1300000
#define MAX_WORD_LENGTH 50
#define DEBUG_MODE 0// if DEBUG_MODE=1, prints the output status at different operation stages.

int reducer_stop_flag=0;

struct word{
	char *key;//defines the actual word (which is supposed to be of maximum 50 letters)
};
struct word word_hash[NUM_REDUCER][REDUCER_QUEUE_SIZE];
int buffer_status[NUM_REDUCER];//1=buffer full, 0=buffer empty.

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
	printf("Reducer=%d\n",reducer_id);
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
void reducer_thread(FILE *fs, int F,int thread_id, int *mapper_stop_flag, int i, int k, int find_flag, char *file_name, char *file_id, char *file_ext, char *file_dir, struct word reducer_word_list[],int *reducer_word_count, unsigned int current_reducer_word_length, unsigned int current_reducer_unique_words, struct word temp_realloc_array, unsigned int reducer_total_word){
	thread_id = omp_get_thread_num();
	char *word_end = "\n";
	char *separator = "=";
	current_reducer_unique_words = 0;
	current_reducer_word_length = REDUCER_QUEUE_SIZE;
	reducer_word_list = malloc(sizeof(struct word)*REDUCER_QUEUE_SIZE);
	for (i=0;i<REDUCER_QUEUE_SIZE;i++){
		reducer_word_list[i].key = malloc(MAX_WORD_LENGTH*sizeof(char));
	}
	reducer_word_count = malloc(sizeof(int)*REDUCER_QUEUE_SIZE);
	for (i=0;i<REDUCER_QUEUE_SIZE;i++){
		reducer_word_count[i] = 0;
	}
	printf("Reducer ID =  %d,Thread ID = %d\n",F, thread_id);
	F = F-1;//thread id start from 0, but in our code, it starts from 1. So, to adjust, subtract 1

	//Read the unique buffer elements into the local reducer list - reducer_word_list[reducer_id][]
	for (i=0;i<REDUCER_QUEUE_SIZE;i++){
		if (current_reducer_unique_words==0){
			if (strlen(word_hash[F][i].key) > 0){
				strcpy(reducer_word_list[0].key,word_hash[F][i].key);
				reducer_word_count[0]++;
				current_reducer_unique_words++;
			}
		}
		else{
			if (strlen(word_hash[F][i].key) > 0){
				find_flag = 0;
				for (k=0;k<current_reducer_unique_words;k++){
					if (!strcmp(word_hash[F][i].key,reducer_word_list[k].key)){
						find_flag=1;
						break;
					}
				}
				if (find_flag==1){
					reducer_word_count[k]++;
				}
				else if (find_flag==0){
					strcpy(reducer_word_list[current_reducer_unique_words].key,word_hash[F][i].key);
					reducer_word_count[current_reducer_unique_words]++;
					current_reducer_unique_words++;
				}
			}
		}
	}

	strcpy(file_dir,"../Results/Serial_Reducer_");
	strcpy(file_ext,".txt");
	sprintf(file_id,"%d",F);
	strcpy(file_name,strcat(file_id,file_ext));
	strcpy(file_name,strcat(file_dir,file_name));
	if (DEBUG_MODE==1){
		printf("%s\n",file_name);
	}
	fs = fopen(file_name, "w");
	if (fs == NULL){
		printf("Error in opening file, Thread = %d, File = %s\n",thread_id,file_name);
		return;
	}
	reducer_total_word = 0;
	for (i=0;i<current_reducer_unique_words;i++){
		fputs(reducer_word_list[i].key, fs);
		fputs(separator,fs);
		fprintf(fs, "%d", reducer_word_count[i]);
		reducer_total_word += reducer_word_count[i];
		fputs(word_end,fs);
	}
	fclose(fs);
	free(reducer_word_list);
	free(reducer_word_count);
	printf("Exiting Reducer=%d, Thread ID = %d, Unique_Words = %d, Total Words = %d\n",F,thread_id,current_reducer_unique_words,reducer_total_word);
	return;
}

void read_file(int F, FILE *fp, int thread_id,unsigned int word_count, int actual_number, char *file_name, char *file_id, char *file_ext, char *file_dir, char *p, char *buff, int verbose, char *delim,int reducer_id,int hash_number, int i, int tt, char *temp,int word_flag, int *mapper_stop_flag, struct word word_hash[][REDUCER_QUEUE_SIZE],int *reducer_word_count, int *buffer_status, int *buffer_word_count){
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

	buff = (char*)malloc(REDUCER_QUEUE_SIZE * sizeof(char));
	while((fgets(buff, REDUCER_QUEUE_SIZE, (FILE*)fp)) != NULL){
		p = (char*)malloc(REDUCER_QUEUE_SIZE * sizeof(char));
		//copy the buffer content to a dynmamic array
		strcpy(p,buff);
		//Find the words in the sentence, which is in p and generate their hash codes and reducer id
		while(*p){
			if (*p == ' ' || *p == '\n' || *p == '\t' || *p == ',' || *p == ';' || *p == '.' || *p == ':' || *p == '{' || *p == '}' || *p == '[' || *p == ']' || *p == '(' || *p == ')' || *p == '-' || *p == '=' || *p == '?' || *p == '!'){
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
					//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					buffer_word_count[reducer_id]++;
					strcpy(word_hash[reducer_id][buffer_word_count[reducer_id]].key,temp);
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
	//if (DEBUG_MODE==1){
		printf("Thread_ID=%d, Total Words = %d, File Name = %s\n",thread_id,word_count,file_name);
	//}
	fclose(fp);
	mapper_stop_flag[F-1] = 1;
	free(buff);
	return;
}
int main(int argc, char *argv[]){
	system("exec rm -r ../Results/Serial*");//delete the existing results

	int find_flag = 0;
	struct word temp_realloc_array;
	struct word *reducer_word_list;
	int buffer_word_count[NUM_REDUCER];
	unsigned int reducer_total_word = 0;
	int *reducer_word_count;
	unsigned int current_reducer_unique_words = 0;
	unsigned int current_reducer_word_length = REDUCER_QUEUE_SIZE;
	int mapper_stop_flag[NUM_FILES];
	int Front=-1;//reducer queue variable
	int Rear=-1;////reducer queue variable
	int F;
	int i,k;
	char temp[100];
	double start_s,start_p,end_s,end_p;
	unsigned int word_count=0;
	FILE *fp;
	FILE *fs;//save files after reducing
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
	buff = (char*)malloc(REDUCER_QUEUE_SIZE * sizeof(char));

	for (i=0;i<NUM_FILES;i++){
		mapper_stop_flag[i] = 0;
	}
	for (i=0;i<NUM_REDUCER;i++){
		for (k=0;k<REDUCER_QUEUE_SIZE;k++){
			word_hash[i][k].key = malloc(MAX_WORD_LENGTH*sizeof(char));
		}
	}
	for (i=0;i<NUM_REDUCER;i++){
		buffer_word_count[i] = 0;
	}
	for (i=0;i<NUM_REDUCER;i++){
		buffer_status[i] = 0;//initially, the reducer buffers are empty
	}
	i=k=0;
	printf("Allocation Done\n");

	// Serial code
	printf("\n\n\n");
	start_s = omp_get_wtime();
	for (F=1;F<=NUM_FILES;F++){
		read_file(F,fp,thread_id,word_count,actual_number,file_name,file_id,file_ext,file_dir,p,buff,verbose,delim,reducer_id,hash_number,i,tt,temp,word_flag,mapper_stop_flag,word_hash,reducer_word_count,buffer_status,buffer_word_count);

	}
	for (F=1;F<=NUM_REDUCER;F++){
		reducer_thread(fs,F,thread_id,mapper_stop_flag,i,k,find_flag,file_name,file_id,file_ext,file_dir,reducer_word_list,reducer_word_count,current_reducer_word_length,current_reducer_unique_words,temp_realloc_array,reducer_total_word);
	}
	end_s = omp_get_wtime();
	printf("\n\n\n");



	printf("\n\n\n");
	printf("Reduction Complete\n");
	printf("Time for Sequential Run = %f s\n",end_s-start_s);
	return 0;
}
