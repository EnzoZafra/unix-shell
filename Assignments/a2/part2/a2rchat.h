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
  E_NO_FD = 6,
  E_WAIT_FAIL = 7,
  E_CLKTCK = 8
} error_t;

void print_error(int errorcode);
#endif
