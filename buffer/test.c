#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define DATA_LENGTH 1024
#define RUNS 100000
#define MICROTOMILLI 1000
// decimal equivalent of chars 0-9 for ASCII
#define ZERO_ASCII 48
#define NINE_ASCII 57
// system calls
#define __NR_init_buffer_421 442
#define __NR_enqueue_buffer_421 443
#define __NR_dequeue_buffer_421 444
#define __NR_delete_buffer_421 445

long init_buffer_syscall(void){
	return syscall(__NR_init_buffer_421);
}

long enqueue_buffer_syscall(char* data){
	return syscall(__NR_enqueue_buffer_421, data);
}

long dequeue_buffer_syscall(char* data){
	return syscall(__NR_dequeue_buffer_421, data);
}

long delete_buffer_syscall(void){
	return syscall(__NR_delete_buffer_421);
}

// prototypes for consumer and producer
void* producerRoutine(void* arg);
void* consumerRoutine(void* arg);

int main(int argc, char *argv[]){
	pthread_t producer, consumer;
	char edgeCase[DATA_LENGTH];

	// edge cases for enqueueing, dequeueing, and deleting 
	// on a nonexistent buffer
	if(enqueue_buffer_syscall(edgeCase) < 0)
		perror("Enqueue syscall failed.\n");
	else
		printf("Enqueue buffer syscall ran successfully, check dmesg output\n");

	
	if(dequeue_buffer_syscall(edgeCase) < 0)
		perror("Dequeue syscall failed.\n");
	else
		printf("Dequeue buffer syscall ran successfully, check dmesg output\n");


	if(delete_buffer_syscall() < 0)
		perror("Delete syscall failed.\n");
	else
		printf("Delete buffer syscall ran successfully, check dmesg output\n");

	// initialize buffer
	if(init_buffer_syscall() < 0)
		perror("Initialize syscall failed.\n");
	else
		printf("Initialize buffer syscall ran successfully, check dmesg output\n");

	// initialize the threads and call the respective functions
	pthread_create(&producer, NULL, producerRoutine, NULL);
	pthread_create(&consumer, NULL, consumerRoutine, NULL);
	// threads finish
	pthread_join(producer, NULL);
	pthread_join(consumer, NULL);

	// delete buffer
	if(delete_buffer_syscall() < 0)
		perror("Delete syscall failed.\n");
	else
		printf("Delete buffer syscall ran successfully, check dmesg output\n");

	return 0;
}

void* producerRoutine(void* arg){
	char data[DATA_LENGTH];
	int iterations, sleepTime, blockChar = ZERO_ASCII;

	// do enqueue 100k times 
	for(iterations = 0; iterations < RUNS; iterations++){
		// ensure the number put into data is a single digit
		if(blockChar > NINE_ASCII)
			blockChar = ZERO_ASCII;

		// typecast decimal to char value in ASCII and add to data
		char addChar = (char) blockChar;
		memset(data, addChar, 1024);
		data[1023] = '\0';
		
		// loop the decimal value for char conversion
		blockChar++;

		// randomly wait between 0-10 milliseconds prior to each insert
		srand(time(NULL));
		sleepTime = rand() % 11;
		usleep(sleepTime * MICROTOMILLI);

		if(enqueue_buffer_syscall(data) < 0)
			perror("Enqueue syscall failed.\n");
		else
			printf("Enqueue buffer syscall ran successfully, check dmesg output\n");
	}	
}

void* consumerRoutine(void* arg){
	char data[DATA_LENGTH];
	int iterations, sleepTime;

	// pass in empty data string to use to print in dequeue later
	for(iterations = 0; iterations < RUNS; iterations++){

		// randomly wait between 0-10 milliseconds prior to each dequeue
		srand(time(NULL));
		sleepTime = rand() % 11;
		usleep(sleepTime * MICROTOMILLI);

		if(dequeue_buffer_syscall(data) < 0)
			perror("Dequeue syscall failed.\n");
		else
			printf("Dequeue buffer syscall ran successfully, check dmesg output\n");
	}
}
