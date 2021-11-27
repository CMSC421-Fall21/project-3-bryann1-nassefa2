#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "buffer_user.c"

#define RUNS 100000
#define MICROTOMILLI 1000
// decimal equivalent of chars 0-9 for ASCII
#define ZERO_ASCII 48
#define NINE_ASCII 57

// prototypes for consumer and producer
void* producerRoutine(void* arg);
void* consumerRoutine(void* arg);

int main(int argc, char *argv[]){
	pthread_t producer, consumer;
	char edgeCase[DATA_LENGTH];
	enqueue_buffer_421(edgeCase);
	dequeue_buffer_421(edgeCase);
	delete_buffer_421();
	init_buffer_421();
	print_semaphores();
	// initialize the threads and call the respective functions
	pthread_create(&producer, NULL, producerRoutine, NULL);
	pthread_create(&consumer, NULL, consumerRoutine, NULL);
	// threads finish
	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);
	print_semaphores();
	sem_destroy(&mutex);
	sem_destroy(&fill_count);
	sem_destroy(&empty_count);
	delete_buffer_421();
	return 0;
}

void* producerRoutine(void* arg){
	char data[DATA_LENGTH];
	int iterations, sleepTime, blockChar = ZERO_ASCII;

	// do enqueue 100k times 
	for(iterations = 0; iterations < RUNS; iterations++){
		// make sure char stays as a single digit from 0-9
		if(blockChar > NINE_ASCII)
			blockChar = ZERO_ASCII;

		// add chars to data
		char addChar = (char) blockChar;
		memset(data, addChar, 1024);
		data[1023] = '\0';	

		// loop the decimal value for char conversion
		blockChar++;

		// randomly wait between 0-10 milliseconds prior to each insert
		srand(time(NULL));
		sleepTime = rand() % 101;
		usleep(sleepTime * MICROTOMILLI);

		enqueue_buffer_421(data);
	}
}

void* consumerRoutine(void* arg){
	char data[DATA_LENGTH];
	int iterations, sleepTime;

	// pass in empty data string to use to print in dequeue later
	for(iterations = 0; iterations < RUNS; iterations++){

		// randomly wait between 0-10 milliseconds prior to each dequeue
		srand(time(NULL));
		sleepTime = rand() % 101;
		usleep(sleepTime * MICROTOMILLI);

		dequeue_buffer_421(data);
	}
}
