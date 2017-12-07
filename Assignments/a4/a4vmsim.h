/**
  *
  * a4vmsim.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdlib.h>

#ifndef a4vmsim_h
#define a4vmsim_h

typedef enum {
  E_USG,
  E_PAGESIZE,
  E_MEMSIZE,
  E_STRAT,
  E_OP_PARSE
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
uint32_t roundNearMult(uint32_t value, int multipleof);
void simulate(int pagesize, uint32_t memsize, strat_t strategy);
void print_output(char* strategy, double elapsed);
int parse_operation(char ref_string[], strat_t strat);
void init(int pagesize, uint32_t memsize);
bool check_pmem(uint32_t v_addr);
void handle_pfault(strat_t strat);

void inc_acc(char oper_byte);
void dec_acc(char oper_byte);
int write_op(uint32_t pnum, strat_t strat);
int read_op(uint32_t pnum, strat_t strat);

#endif
