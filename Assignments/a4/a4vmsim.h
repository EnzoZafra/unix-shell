/**
  *
  * a4vmsim.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */


#ifndef a4vmsim_h
#define a4vmsim_h

typedef enum {
  E_USG,
  E_PAGESIZE,
  E_MEMSIZE
} error_t;

void print_error(int errorcode);
bool ispowerof2(unsigned int x);
int roundNearMult(int value, int multipleof);

#endif
