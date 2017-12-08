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

void init_ptable(uint32_t size);
uint32_t initEntry(uint32_t v_addr);
uint32_t getEntry(uint32_t virtual_addr);
uint32_t ptable_len();
void addEntry(uint32_t virtual_addr);
bool swap(uint32_t v_addr1, uint32_t v_addr2);

#endif
