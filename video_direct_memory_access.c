#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "pipe.h"

#define STDOUT 1
#define START 0x22CE0000
#define READ_FROM_MEM_LENGTH 0x7430
#define CHECK_FOR_CHANGES_SIZE 16
#define CHECK_FOR_CHANGES_OFFSET 64

void* read_memory(void* arg);
void* write_stream(void* arg);

typedef struct {
	pipe_producer_t* producer;
	pipe_consumer_t* consumer;
} thread_context_t;

void* read_memory(void* arg) {
	thread_context_t* thread_context = (thread_context_t*) arg;
	char checkForChanges[CHECK_FOR_CHANGES_SIZE] = {0};
	int fd = open("/dev/mem", O_RDWR|O_SYNC);
	int* addr = (int *)mmap(0, READ_FROM_MEM_LENGTH, PROT_READ, MAP_SHARED, fd, START);
	
	while(1) {
		if(memcmp(addr+CHECK_FOR_CHANGES_OFFSET, checkForChanges, CHECK_FOR_CHANGES_SIZE) != 0) {
			pipe_push(thread_context->producer, addr, 1);
			memcpy(checkForChanges, addr+CHECK_FOR_CHANGES_OFFSET, CHECK_FOR_CHANGES_SIZE);
		}
	}
	
	return NULL;
}

void* write_stream(void* arg) {
	thread_context_t* thread_context = (thread_context_t*) arg;
	char block[READ_FROM_MEM_LENGTH];
	int nowarn;
	
	while(1) {
		nowarn = pipe_pop(thread_context->consumer, block, 1);
		write(STDOUT, block, READ_FROM_MEM_LENGTH);
	}
	
	return NULL;
}

int main(int argc, char** argv)
{
	pipe_t* pipe = pipe_new(READ_FROM_MEM_LENGTH, 0);
	
	pipe_producer_t* producer = pipe_producer_new(pipe);
	pipe_consumer_t* consumer = pipe_consumer_new(pipe);
	
	thread_context_t thread_context = {
		.producer = producer,
		.consumer = consumer
	};
	
	pthread_t producerThread, consumerThread;
	
	pthread_create(&producerThread, NULL, read_memory, (void*) &thread_context);
	pthread_create(&consumerThread, NULL, write_stream, (void*) &thread_context);
	
	pipe_free(pipe);
	
	pthread_join(producerThread, NULL);
	pthread_join(consumerThread, NULL);
	
	pipe_producer_free(producer);
	pipe_consumer_free(consumer);
	
	return 0;
}
