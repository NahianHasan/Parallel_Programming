#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NUM_REDUCER 8
#define HASH_BIT 8

struct word{
	char key[50];//defines the actual word (which is supposed to be of maximum 50 letters)
	int hash;//hash code for that word
	int reducer;//reducer id
};


int * decimal_to_binary(int decimal){
	static int binary[HASH_BIT];//8 bit binary representation
	int i=HASH_BIT-1;
	while(decimal>0){
		binary[i--] = decimal % 2;
		decimal /= 2;
	}
	return binary;
}
void extract_letter_from_word(char *str){
	char letter[1];
	int *binary;
	int word_length = strlen(str);
	for (int i=0; i<word_length; i++){
		binary = decimal_to_binary(str[i]);
		printf("%c --> %d --> ",str[i],str[i]);
		for (int j=0;j<HASH_BIT;j++){
			printf("%d",binary[j]);
		}
		printf("\n");
	}
	return;
}
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

	hash_number = str[0];
	for (int i=1; i<word_length; i++){
		//binary = decimal_to_binary(str[i]);
		hash_number ^= str[i];
	}
	return hash_number;
}

void print_hash_codes(struct word unit_word){
	//print the results in a specific format in the IO
	int te;
	printf("%s",unit_word.key);
	for (int k=0; k<abs((20-strlen(unit_word.key))); k++){
		printf("-");
	}
	printf("H=%d",unit_word.hash);
	if (unit_word.hash < 10){
		te = 1;
	}
	else if (unit_word.hash < 100){
		te = 2;
	}
	else if (unit_word.hash < 1000){
		te = 3;
	}
	for (int k=0; k<abs(10-te); k++){
		printf("-");
	}
	printf("R=%d\n",unit_word.reducer);
	return;
}
int extract_word_from_sentence(char *str, int verbose){
	char *p;
	char temp[100];
	char word[100];
	int flag=0;
	int tt = 0;
	int word_flag=0;
	int actual_number = 0;
	/*
	char delim[] = " ,\t\r;";
	char *p = strtok(str,delim);
	int hash_number;
	int actual_number = 0;

	while(p != NULL){
		struct word unit_word;
		hash_number = extract_hash_encoding(p);
		if (hash_number != 10){//removing Data Link Escape
			strcpy(unit_word.key,p);
			unit_word.hash = hash_number;
			unit_word.reducer = hash_number % NUM_REDUCER;
			if (verbose==1){
				print_hash_codes(unit_word);
			}
			actual_number += 1;
		}
		p = strtok(NULL,delim);
	}
	*/

	p = (char*)malloc(100 * sizeof(char));
	strcpy(p,str);
	printf("%s\n",p);
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
			printf("Temp = %s--->",temp);
			hash_number = extract_hash_encoding(temp);
			if (hash_number != 10){//removing Data Link Escape
				reducer_id = hash_number % NUM_REDUCER;
				if (verbose==1){
					print_hash_codes(temp,hash_number,reducer_id);
				}
				actual_number += 1;
			}
			tt=0;
			memset(temp,0,sizeof(temp));
		}
		++p;
	}
	return actual_number;
}



int main(int argc, char *argv[]){
	FILE *fp;
	char buff[1000];
	char st[10];
	unsigned int word_count=0;
	int actual_number;
	int verbose=1;//whether to show the hashcodes

	fp = fopen("../files/1.txt","r");
	if (fp == NULL){
		printf("Error in opening file\n");
		return(-1);
	}

	//fscanf(fp,"%s",buff);
	while((fgets(buff, 1000, (FILE*)fp)) != NULL){
		actual_number = extract_word_from_sentence(buff,verbose);
		word_count += actual_number;
	}

	printf("Total Words = %d\n",word_count);
	fclose(fp);

	return 0;
}
