/**
  *
  * queue.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdbool.h>
#include <stdint.h>

#ifndef queue_h
#define queue_h

typedef struct {
	uint32_t *arr;
	uint32_t head;
	uint32_t tail;
	uint32_t size;
} t_queue;

t_queue *createQueue(uint32_t size);
void deallocateQueue(t_queue *q);
void enqueue(t_queue *q, uint32_t n);
uint32_t dequeue(t_queue *q);
bool is_full(t_queue *q);
bool empty(t_queue *q);

#endif
