/**
  *
  * strategy.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "strategy.h"
#include "a4vmsim.h"
#include "pagetable.h"


// "none" strategy
void none_handler() {
  // TODO
}

// "mrand" strategy
void mrand_handler(uint32_t numframes, uint32_t last3_refs[]) {
  uint32_t pmem_idx, v_addr;
  bool findingEvict = true;

  while(findingEvict) {
    pmem_idx = limited_rand(numframes);
    v_addr = memory[pmem_idx];
    findingEvict = false;

    for (int i = 0; i < 3; i++) {
      if (v_addr == last3_refs[i]) {
        findingEvict = true;
        break;
      }
    }
  }

  t_ptentry* evictedPage = getEntry(v_addr);
  evict_page(pmem_idx, evictedPage) ;
}

// "lru" strategy
void lru_handler() {
  // TODO
}

// "sec" strategy
void sec_handler() {
  // TODO
}

uint32_t limited_rand(uint32_t limit)
{
  uint32_t r, d = RAND_MAX / limit;
  limit *= d;
  do { r = rand(); } while (r >= limit);
  return r / d;
}
