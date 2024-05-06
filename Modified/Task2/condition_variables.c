#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#define SIZE 12  //Size of the Circular Queue

pthread_cond_t prod;
pthread_cond_t cons;
pthread_mutex_t mutex;

char circQueue[SIZE];
int front = -1;
int back = -1;
bool endOfFile = false;


//Checks to see if the queue is full
bool queueFull(){
    return ((front == 0 && back == SIZE - 1) || (back + 1) % SIZE == front);
}

//Adds an element to the Circular Queue
void insertIntoQueue(char data){

    if (queueFull()){

        //The Queue is full. Cannot insert more data
        printf("Queue cannot be written to because it is full.\n");
        return;
    }
    else if (front == -1){

        front = 0;
        back = 0;
    }
    else if (back == SIZE - 1 && front != 0)

        back = 0;

    else back++;

    circQueue[back] = data;
    return;
}

//Removes an element from the Circular Queue and returns its value
char readFromQueue(){

    if ((front == -1)){

        // Queue is empty
        printf("Nothing in queue!\n");
        return '0';
    }

    char data = circQueue[front];
    circQueue[front] = '0';

    if (front == back){

        front = -1;
        back = -1;
    }
    else if (front == SIZE - 1) front = 0;

    else front++;

    return data;
}

//Function responsible for the behavior of the producer thread
void *producer_thread_function( void *arg) {

    FILE *fp;

    fp = fopen("message.txt", "r");

    if (fp == NULL){
        exit(1);
    }
    char c;
    while ((c = fgetc(fp)) != EOF) {

        while(queueFull() || pthread_mutex_trylock(&mutex) == -1)
            pthread_cond_wait(&prod, &mutex);

        insertIntoQueue(c);
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cons);
    }

    fclose(fp); 
    endOfFile = true;
    return NULL;
}

//Function responsible for the behavior of the consumer thread
void *consumer_thread_function(void *arg){

    char current;

    while (!endOfFile || front != -1) {

        while(front == -1 || pthread_mutex_trylock(&mutex) == -1)
            pthread_cond_wait(&cons, &mutex);

        current = readFromQueue();

        pthread_mutex_unlock(&mutex);
        printf("%c", current);
        pthread_cond_signal(&prod);

    }
    return NULL;
}

int main(void){

    pthread_t thread_ID1, thread_ID2;
    int i;

    //Initialize the condition variables and the mutex
    pthread_cond_init(&prod, NULL);
    pthread_cond_init(&cons, NULL);
    pthread_mutex_init(&mutex, NULL);

    //Create the consumer and producer threads
    pthread_create(&thread_ID1, NULL, producer_thread_function, NULL);
    pthread_create(&thread_ID2, NULL, consumer_thread_function, NULL);

    //Ensure that main() doesn't terminate before the threads
    pthread_join(thread_ID1, NULL);
    pthread_join(thread_ID2, NULL);

    //Destroy the conditional variables and mutex because they are not needed
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&prod);
    pthread_cond_destroy(&cons);

    return 0;
}

