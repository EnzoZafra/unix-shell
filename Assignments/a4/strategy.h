/**
  *
  * strategy.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdlib.h>

#ifndef strategy_h
#define strategy_h

uint32_t none_handler();
uint32_t mrand_handler();
uint32_t lru_handler();
uint32_t sec_handler();
uint32_t limited_rand(uint32_t limit);

#endif
