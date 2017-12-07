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
  ((t_ptentry*)pagetable[len])->virtual_addr = v_addr;
  t_ptentry* out = (t_ptentry*)pagetable[len];
  len++;
  return out;
}

void init_ptable(int max_size) {
  pagetable = (t_ptentry**)malloc(max_size * sizeof(*pagetable));
  /* pagetable = (t_ptentry**)malloc(max_size, sizeof(*pagetable)); */

  for (int i = 0; i < max_size; i++) {
    t_ptentry* tmp = (t_ptentry*) malloc(sizeof(t_ptentry*));
    tmp->valid = 0;
    tmp->reference_bit = 0;
    tmp->modified = 0;
    tmp->virtual_addr = -1;
    tmp->physical_addr = -1;
    pagetable[i] = tmp;
    /* ((t_ptentry*)pagetable[i])->valid = 0; */
    /* ((t_ptentry*)pagetable[i])->reference_bit = 0; */
    /* ((t_ptentry*)pagetable[i])->modified = 0; */
    /* ((t_ptentry*)pagetable[i])->virtual_addr = -1; */
    /* ((t_ptentry*)pagetable[i])->physical_addr = -1; */
  }
}

uint32_t ptable_len() {
  return len;
}

t_ptentry* getEntry(uint32_t v_addr) {
  for (int i = 0; i < len; i++) {
    t_ptentry* out = (t_ptentry*)pagetable[i];
    if (out->virtual_addr == v_addr) {
      return out;
    }
  }
  // If not found, add the page entry into the table
  return initEntry(v_addr);
}
