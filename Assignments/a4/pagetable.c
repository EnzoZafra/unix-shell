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

uint32_t initEntry(uint32_t v_addr) {
  uint32_t tmp = len;
  pagetable[len]->virtual_addr = v_addr;
  len++;
  //TODO:
  printf("not found in ptable, new index: %i\n", tmp);
  return tmp;
}

void init_ptable(uint32_t max_size) {
  pagetable = (t_ptentry**)malloc(max_size * sizeof(*pagetable));
  /* pagetable = (t_ptentry**)malloc(max_size, sizeof(*pagetable)); */

  for (uint32_t i = 0; i < max_size; i++) {
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

uint32_t getEntry(uint32_t v_addr) {
  if (v_addr == 0) {
    printf("ZERO HERE \n");
    exit(1);
  }
  for (int i = 0; i < len; i++) {
    t_ptentry* out = pagetable[i];
    if (out->virtual_addr == v_addr) {
      return i;
    }
  }
  // If not found, add the page entry into the table
  return initEntry(v_addr);
}
