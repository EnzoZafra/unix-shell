/**
  *
  * queue.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdlib.h>
#include <stdio.h>

#include "queue.h"

t_queue *createQueue(uint32_t size) {
  t_queue *q = malloc(sizeof(t_queue));
  q->arr = malloc(sizeof(uint32_t) * size);
  q->size = size;
  q->head = 0;
  q->tail = 0;

  return q;
}

void deallocateQueue(t_queue *q) {
  free(q->arr);
  free(q);
}

bool full(t_queue *q) {
  return (q->head == ((q->tail + 1) % q->size));
}

bool empty(t_queue *q) {
  return (q->tail == q->head);
}

void enqueue(t_queue *q, uint32_t n) {
  if (full(q)) {
    printf("queue is full!");
    exit(0);
  }

  q->arr[q->tail] = n;
  q->tail = (q->tail + 1) % q->size;
}

uint32_t dequeue(t_queue *q) {
  if (empty(q)) {
    printf("queue is empty");
    exit(0);
  }

  uint32_t temp = q->arr[q->head];
  q->head = (q->head + 1) % q->size;
  return temp;
}
