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
uint32_t none_handler() {
  // Dont have to do any eviction for none.
  return -1;
}

// "mrand" strategy
uint32_t mrand_handler(uint32_t numframes) {
  uint32_t pmem_idx = -1, v_addr = -1;
  bool findingEvict = true;

  while(findingEvict) {
    pmem_idx = limited_rand(numframes);
    v_addr = memory[pmem_idx];

    if (v_addr == -1) {
      print_error(E_NOT_IN_PMEM);
    }

    // Check the stack which holds the last 3 refs
    if (search(&head, v_addr) == NULL) {
      findingEvict = false;
    }
  }

  uint32_t evictedPage = getEntry(v_addr);
  evict_page(pmem_idx, evictedPage);
  return pmem_idx;
}

// "lru" strategy
uint32_t lru_handler() {
  // Least recently used will be in tail of stack
  uint32_t v_addr = delTail(&head, &tail);
  uint32_t evictedPage = getEntry(v_addr);

  uint32_t pmem_idx = check_pmem(v_addr);
  if (pmem_idx == -1) {
    print_error(E_NOT_IN_PMEM);
  }
  evict_page(pmem_idx, evictedPage);
  return pmem_idx;
}

// "sec" strategy
uint32_t sec_handler() {
  // TODO
  return -1;
}

uint32_t limited_rand(uint32_t limit)
{
  uint32_t r, d = RAND_MAX / limit;
  limit *= d;
  do { r = rand(); } while (r >= limit);
  return r / d;
}
