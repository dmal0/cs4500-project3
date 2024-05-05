#define _GNU_SOURCE // needed to compile in centos

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 1024 // From original
#define NUM_THREADS 4 // Num threads to split between

// Original variables
int total = 0;
int n1,n2;
char *s1,*s2;
FILE *fp;

// Mutex
pthread_mutex_t mutex_lock;

int readf(FILE *fp)
{
	if((fp=fopen("strings.txt", "r"))==NULL){
		printf("ERROR: can't open string.txt!\n");
		return 0;
	}
	s1=(char *)malloc(sizeof(char)*MAX);
	if(s1==NULL){
		printf("ERROR: Out of memory!\n");
		return -1;
	}
	s2=(char *)malloc(sizeof(char)*MAX);
	if(s2==NULL){
		printf("ERROR: Out of memory\n");
		return -1;
	}
	/*read s1 s2 from the file*/
	s1=fgets(s1, MAX, fp);
	s2=fgets(s2, MAX, fp);
	n1=strlen(s1)-1;  /*length of s1*/
	n2=strlen(s2)-1; /*length of s2*/
	
	if(s1==NULL || s2==NULL || n1<n2)  /*when error exit*/
		return -1;
}

// Modify from sequential num_substring
// Pass in threads to check different substrings,
// then update the global total
int *num_substring(void *arg)
{
	int i,j,k; // Loop counters
	int count; // Count for match string length
	int start = *((int *)arg); // Start of s1 subrange
	int end = (start + n1) / NUM_THREADS; // End of s1 subrange

	for (i = start; i <= end; i++){   
		count=0;
		for(j = i,k = 0; k < n2; j++,k++){  //search for the next string of size of n2 
			if (*(s1 + j) == *(s2 + k)){ // Matching characters
				count++;
				
				if(count == n2){ // Match length check
				pthread_mutex_lock(&mutex_lock);
				total++;		//find a substring in this step   
				pthread_mutex_unlock(&mutex_lock);    
				}
				count = 0; // Reset count
				//break;
			}
			else{
				count = 0; // Reset count
			}                
		}
	}
	
	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
	//int count; 
	int i;
	pthread_t threads[NUM_THREADS];
	int threadArgs[NUM_THREADS];

	pthread_mutex_init(&mutex_lock, NULL);
 
	readf(fp);

	for (i = 0; i < NUM_THREADS; i++)
	{
		// Length of s1 divided by number of threads
		// Use multiply by i to check different subranges of s1
		// Each thread checks a different subrange
		threadArgs[i] = i * (n1 / NUM_THREADS);
		pthread_create(&threads[i], NULL, num_substring, (void *)&threadArgs[i]);
	}

	//count = num_substring();
 	printf("The number of substrings is: %d\n", total);

	pthread_mutex_destroy(&mutex_lock);

	return 1;
}











