/**
  *
  * a4vmsim.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdlib.h>
#include "pagetable.h"
#include "stack.h"

#ifndef a4vmsim_h
#define a4vmsim_h

typedef enum {
  E_USG,
  E_PAGESIZE,
  E_MEMSIZE,
  E_STRAT,
  E_OP_PARSE,
  E_PMEM_OVERFLOW,
  E_NOT_IN_PMEM,
  E_MRAND_3REFS
} error_t;

typedef enum {
  NONE,
  MRAND,
  LRU,
  SEC
} strat_t;

typedef struct output {
  uint32_t memrefs;
  uint32_t writes;
  int pagefaults;
  uint32_t flushes;
  uint32_t acc;
} t_output;

#define SYS_BITS 32
extern uint32_t* memory;
extern node* head;
extern node* tail;

void print_error(int errorcode);
bool ispowerof2(uint32_t x);
uint32_t roundNearMult(uint32_t value, int multipleof);
void simulate(int pagesize, uint32_t memsize, strat_t strategy);
void print_output(char* strategy, double elapsed);
int parse_operation(char ref_string[], strat_t strat);
void init(int pagesize, uint32_t memsize);
uint32_t check_pmem(uint32_t v_addr);
int check_fault(uint32_t refpage_idx, strat_t strat);
void evict_page(uint32_t index, uint32_t page_idx);
void load_page(uint32_t avail_index, uint32_t page_idx, strat_t strat);
uint32_t handle_pfault(strat_t strat);

void inc_acc(char oper_byte);
void dec_acc(char oper_byte);
int write_op(uint32_t pnum, strat_t strat);
int read_op(uint32_t pnum, strat_t strat);

#endif
