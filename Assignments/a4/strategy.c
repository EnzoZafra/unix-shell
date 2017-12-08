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

extern t_ptentry** pagetable;
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
  uint32_t v_addr = -1;
  // Tail because we're using the stack as a queue.
  node* tmp = tail;
  // Traverse (queue) look for page with ref bit = 0
  while (tmp != NULL) {
    v_addr = tmp->data;
    t_ptentry* page = pagetable[getEntry(v_addr)];
    // Found a page w/ ref_bit
    if (page->reference_bit == 0) {
      break;
    }
    // If ref bit is 1, set it to 0
    else {
      page->reference_bit = 0;
    }
    tmp = tmp->above;

    // When we've gone full circle go back again
    if (tmp == NULL) {
      tmp = tail;
    }
  }

  // Delete it from the queue and evict from phys memory
  v_addr = del(&head, &tail, tmp);
  uint32_t evictedPage = getEntry(v_addr);
  uint32_t pmem_idx = check_pmem(v_addr);
  if (pmem_idx == -1) {
    print_error(E_NOT_IN_PMEM);
  }
  evict_page(pmem_idx, evictedPage);
  return pmem_idx;
}

uint32_t limited_rand(uint32_t limit)
{
  uint32_t r, d = RAND_MAX / limit;
  limit *= d;
  do { r = rand(); } while (r >= limit);
  return r / d;
}
