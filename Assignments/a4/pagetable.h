/**
  *
  * pagetable.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdbool.h>

#ifndef pagetable_h
#define pagetable_h

typedef struct ptentry {
  bool valid;
  bool reference_bit;
  bool modified;
  unsigned long virtual_addr;
  unsigned long physical_addr;
} t_ptentry;

#endif
