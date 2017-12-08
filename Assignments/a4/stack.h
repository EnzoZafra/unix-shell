/**
  *
  * stack.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdlib.h>

#ifndef stack_h
#define stack_h

extern uint32_t size;

typedef struct node {
  uint32_t data;
  struct node *above;
  struct node *below;
} node;

node* newNode(uint32_t data);
void push(node** head, node** tail, uint32_t data);
node* pop(node** head);
node* search(node** head, uint32_t data);
uint32_t del(node** head, node** tail, node* tmp);
uint32_t delTail(node** head, node** tail);
void printList(node* root);
void moveToTop(node** head, node** tail, uint32_t data);
uint32_t stack_size();

#endif
