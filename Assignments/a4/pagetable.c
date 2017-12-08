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
#include "a4vmsim.h"

// Global ptable
t_ptentry** pagetable;
uint32_t len;
uint32_t *hashtable;

uint32_t initEntry(uint32_t v_addr) {
  uint32_t tmp = len;
  pagetable[len]->virtual_addr = v_addr;
  len++;
  return tmp;
}

void init_ptable(uint32_t max_size) {
  pagetable = (t_ptentry**)malloc(max_size * sizeof(*pagetable));

  for (uint32_t i = 0; i < max_size; i++) {
    t_ptentry* tmp = (t_ptentry*) malloc(sizeof(t_ptentry*));
    tmp->valid = 0;
    tmp->reference_bit = 0;
    tmp->modified = 0;
    tmp->virtual_addr = i;
    tmp->physical_addr = -1;
    pagetable[i] = tmp;
  }
}

uint32_t ptable_len() {
  return len;
}

uint32_t getEntry(uint32_t v_addr) {
  t_ptentry* out = pagetable[v_addr];
  if (out->virtual_addr == v_addr) {
    return v_addr;
  } else {
    print_error(E_HASHMAP);
  }
  return -1;
}
