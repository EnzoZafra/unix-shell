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

void none_handler();
void mrand_handler();
void lru_handler();
void sec_handler();
uint32_t limited_rand(uint32_t limit);

#endif
