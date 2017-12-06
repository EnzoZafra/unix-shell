/**
  *
  * pagetable.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdio.h>
#include <stdlib.h>

#include "pagetable.h"

// Global ptable
t_ptentry* pagetable;

void init_ptable(int size) {
  // TODO: should make a hashtable that does v_addr -> pagetable index;
  pagetable = malloc(size * sizeof(*pagetable));

  for (int i = 0; i < size; i++) {
    pagetable[i].valid = 0;
    pagetable[i].reference_bit = 0;
    pagetable[i].modified = 0;
    pagetable[i].virtual_addr = -1;
    pagetable[i].physical_addr = -1;
  }
}
