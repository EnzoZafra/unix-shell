/**
  *
  * a2rchat.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 1
  */

#ifndef a2rchat_h
#define a2rchat_h

#include <stdio.h>
#include <stdlib.h>

#define NMAX 5

typedef enum {
  E_USG_S = 1,
  E_USG_C = 2,
  E_USG = 3,
  E_NEGATIVE = 4,
  E_FIFO = 5,
  E_WRITE_IN = 6,
  E_POLL = 7,
  E_WRITE_OUT = 8,
  E_READ = 9,
  E_CONN_OUTFIFO = 10
} error_t;

void print_error(int errorcode);
void write_fifo(int fd, char* msg); // helper function TODO
void read_fifo(int fd, char* buf);
#endif
