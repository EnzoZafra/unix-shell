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
t_ptentry** pagetable;
uint32_t len;

t_ptentry* initEntry(uint32_t v_addr) {
  pagetable[len]->virtual_addr = v_addr;
  t_ptentry* out = pagetable[len];
  len++;
  return out;
}

void init_ptable(int max_size) {
  pagetable = malloc(max_size * sizeof(*pagetable));

  for (int i = 0; i < max_size; i++) {
    pagetable[i]->valid = 0;
    pagetable[i]->reference_bit = 0;
    pagetable[i]->modified = 0;
    pagetable[i]->virtual_addr = -1;
    pagetable[i]->physical_addr = -1;
  }
}

uint32_t ptable_len() {
  return len;
}

t_ptentry* getEntry(uint32_t v_addr) {
  for (int i = 0; i < len; i++) {
    if (pagetable[i]->virtual_addr == v_addr) {
      return pagetable[i];
    }
  }
  // If not found, add the page entry into the table
  return initEntry(v_addr);
}
