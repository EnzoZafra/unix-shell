/**
  *
  * stack.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#ifndef stack_h
#define stack_h

typedef struct node {
  int data;
  struct node *above;
  struct node *below;
} node;

node* newNode(int data);
void push(node** head, node** tail, int data);
node* pop(node** head);
node* search(node** head, int data);
void del(node** head, node** tail, node* tmp);
void delTail(node** head, node** tail);
void printList(node* root);
void moveToTop(node** head, node** tail, int data);

#endif
