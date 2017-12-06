/**
  *
  * pagetable.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdbool.h>
#include <stdlib.h>

#ifndef pagetable_h
#define pagetable_h

typedef struct ptentry {
  bool valid;
  bool reference_bit;
  bool modified;
  uint32_t virtual_addr;
  uint32_t physical_addr;
} t_ptentry;

void init_ptable(int size);
t_ptentry getEntry(unsigned long virtual_addr);
bool swap(unsigned long v_addr1, unsigned long v_addr2);

#endif
