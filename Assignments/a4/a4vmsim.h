/**
  *
  * a4vmsim.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */


#ifndef a4vmsim_h
#define a4vmsim_h

typedef enum {
  E_USG,
  E_PAGESIZE,
  E_MEMSIZE,
  E_STRAT
} error_t;

typedef enum {
  NONE,
  MRAND,
  LRU,
  SEC
} strat_t;

typedef struct output {
  unsigned int memrefs;
  unsigned int writes;
  unsigned int pagefaults;
  unsigned int flushes;
  unsigned int acc;
} t_output;

#define SYS_BITS 32

void print_error(int errorcode);
bool ispowerof2(unsigned int x);
int roundNearMult(int value, int multipleof);
void simulate(int pagesize, int memsize, strat_t strategy);
void print_output(char* strategy, double elapsed);

#endif
