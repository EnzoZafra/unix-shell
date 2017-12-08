  /**
  *
  * a4vmsim.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 4
  */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

#include "stack.h"
#include "strategy.h"
#include "pagetable.h"
#include "a4vmsim.h"

extern t_ptentry** pagetable;
t_output *output;
int page_numbits;
uint32_t* memory;
uint32_t pmem_len = 0;
uint32_t numframes;
strat_t strat = -1;

// Doubly linked list stack for LRU and MRAND
node* head;
node* tail;

int main(int argc, char *argv[]) {
  clock_t timer;

  if (argc < 4 || argc > 4) {
    print_error(E_USG);
  }

  int pagesize = atoi(argv[1]);
  if (pagesize < 256 || pagesize > 8192 || !ispowerof2(pagesize)) {
    print_error(E_PAGESIZE);
  }

  uint32_t memsize = atoi(argv[2]);
  if (memsize <= 0) {
    print_error(E_MEMSIZE);
  }
  memsize = roundNearMult(memsize, pagesize);

  char* tmpstrat = argv[3];
  if (strcmp(tmpstrat, "none") == 0) {
    strat = NONE;
  } else if (strcmp(tmpstrat, "mrand") == 0) {
    strat = MRAND;
  } else if (strcmp(tmpstrat, "lru") == 0) {
    strat = LRU;
  } else if (strcmp(tmpstrat, "sec") == 0) {
    strat = SEC;
  } else {
    print_error(E_STRAT);
  }

  output = (t_output *) calloc(1, sizeof(t_output));
  init(pagesize, memsize);

  timer = clock();
  simulate(pagesize, memsize);
  timer = clock() - timer;

  double elapsed = ((double) timer) / CLOCKS_PER_SEC;
  print_output(tmpstrat, elapsed);
}

void init(int pagesize, uint32_t memsize) {
  numframes = memsize/pagesize;
  page_numbits = SYS_BITS - log2(pagesize);

  init_ptable(pow(2, page_numbits));
  memory = malloc(numframes * sizeof(uint32_t));

  if (strat == LRU || strat == MRAND || strat == SEC) {
    head = NULL;
    tail = NULL;
  }
}

// Main function for the simulator
void simulate(int pagesize, uint32_t memsize) {
  char ref_string[SYS_BITS/8];

  while(read(STDIN_FILENO, ref_string, SYS_BITS/8)) {
    // Order of ref_string from MSB to LSB is:
    // ref[3] ref[2] ref[1] ref[0]

    output->memrefs++;
    output->pagefaults += parse_operation(ref_string);
  }
}

// Parses the reference string for the operation
int parse_operation(char ref_string[]) {
  uint32_t oper_byte = ref_string[0];
  // Shift 6 bits to the right since the opcode
  // resides in the 2 most significant bits
  uint32_t oper_bits = (oper_byte & 0xff) >> 6;

  // concat the page number bits
  uint32_t tmp = ((ref_string[3] & 0xff) << 24)
                | ((ref_string[2] & 0xff) << 16)
                | ((ref_string[1] & 0xff) << 8);

  // extract the page number
  tmp = tmp >> 8;
  uint32_t pNum = tmp & ((1 << page_numbits) - 1);
  uint32_t refpage_idx = getEntry(pNum);

  switch(oper_bits) {
    case 0:
      return inc_acc(refpage_idx, oper_byte);
    case 1:
      return dec_acc(refpage_idx, oper_byte);
    case 2:
      return write_op(refpage_idx);
    case 3:
      return read_op(refpage_idx);
    default:
      print_error(E_OP_PARSE);
      return 0;
  }
}

// Increment Accumulator operation
int inc_acc(uint32_t refpage_idx, char oper_byte) {
  int value = (oper_byte & 0x3F);
  output->acc += value;

  return check_fault(refpage_idx);
}

// Decrement Accumulator operation
int dec_acc(uint32_t refpage_idx, char oper_byte) {
  int value = (oper_byte & 0x3F);
  output->acc -= value;

  return check_fault(refpage_idx);
}

// Write data to page containing reference word
int write_op(uint32_t refpage_idx) {
  output->writes++;
  t_ptentry* ref_page = pagetable[refpage_idx];
  ref_page->modified = 1;

  return check_fault(refpage_idx);
}

// Read data from page containing reference word
int read_op(uint32_t refpage_idx) {
  return check_fault(refpage_idx);
}

int check_fault(uint32_t refpage_idx) {
  int returnval = 0;
  uint32_t pNum = pagetable[refpage_idx]->virtual_addr;
  // If there is a page fault, handle
  if (check_pmem(pNum) == -1 && strat != NONE) {
    // If phys memory is full, need to evict a page
    if (pmem_len == numframes) {
      uint32_t freed = handle_pfault();
      load_page(freed, refpage_idx);
    }
    // if not full, just load the page in
    else if (pmem_len < numframes) {
      load_page(pmem_len, refpage_idx);
    }
    else {
      print_error(E_PMEM_OVERFLOW);
    }
    returnval = 1;
  }
  // if LRU, most recently referenced page must be moved to top
  if (strat == LRU) {
    moveToTop(&head, &tail, pNum);
  }
  // if MRAND, the stack is being used to hold the last k=3 references.
  else if (strat == MRAND) {
    // if we already have 3 references, remove the last one
    if (stack_size() == 3) {
      delTail(&head, &tail);
    }
    else if (stack_size() > 3) {
      print_error(E_MRAND_3REFS);
    }
    push(&head, &tail, pNum);
  }
  else if (strat == SEC) {
    pagetable[refpage_idx]->reference_bit = 1;
  }

  return returnval;
}

uint32_t check_pmem(uint32_t v_addr) {
  for (uint32_t i = 0; i < pmem_len; i++) {
    if(memory[i] == v_addr) {
      return i;
    }
  }
  return -1;
}

void evict_page(uint32_t pmem_idx, uint32_t page_idx) {
  t_ptentry* pageEvicted = pagetable[page_idx];

  // If a page has been modified since it's brought in, inc flushes
  if(pageEvicted->modified == 1) {
    output->flushes++;
  }

  memory[pmem_idx] = -1;
  pageEvicted->valid = 0;
  pageEvicted->reference_bit = 0;
  pageEvicted->modified = 0;
  pageEvicted->physical_addr = -1;
  pmem_len--;
}

void load_page(uint32_t avail_index, uint32_t page_idx) {
  t_ptentry* page = pagetable[page_idx];

  memory[avail_index] = page->virtual_addr;
  page->physical_addr = avail_index;
  page->valid = 1;
  pmem_len++;

  // If LRU strategy, keep a stack of page numbers
  if (strat == LRU) {
    push(&head, &tail, page->virtual_addr);
  }
  // If SEC, we can use the stack as a queue, but we have to dequeue from tail
  else if (strat == SEC) {
    push(&head, &tail, page->virtual_addr);
  }
}

uint32_t handle_pfault() {
  switch (strat) {
    case NONE:
      return none_handler();
      break;
    case MRAND:
      return mrand_handler(pmem_len);
      break;
    case LRU:
      return lru_handler();
      break;
    case SEC:
      return sec_handler();
      break;
  }
}

// Prints the output of the simulator
void print_output(char* strategy, double elapsed) {
  printf("%i references processed using `%s` in %f sec.\n",
      output->memrefs, strategy, elapsed);
  printf("page faults = %i, write count = %i, flushes %i\n",
      output->pagefaults, output->writes, output->flushes);
  printf("accumulator = %i\n", output->acc);
}

// Checks if value is a power of two
bool ispowerof2(uint32_t x) {
   return x && !(x & (x - 1));
}

// Rounds up the value to the nearest multiple of 'multipleof'
uint32_t roundNearMult(uint32_t value, int multipleof) {
  uint32_t tmp = value % multipleof;
  if (tmp == 0) {
    return value;
  }
  return value + multipleof - tmp;
}

// Prints errors to the user depending on the code.
void print_error(int errorcode) {
  switch (errorcode) {
    case E_USG:
      fprintf(stderr, "usage: a4vmsim pagesize memsize strategy\n");
      break;
    case E_PAGESIZE:
      fprintf(stderr, "pagesize must be a power of two between 256 bytes and 8192 bytes\n");
      break;
    case E_MEMSIZE:
      fprintf(stderr, "memsize must be a valid integer\n");
      break;
    case E_STRAT:
      fprintf(stderr, "strategy must be one of [none, mrand, lru, sec]\n");
      break;
    case E_OP_PARSE:
      fprintf(stderr, "failed to parse the operation\n");
      break;
    case E_PMEM_OVERFLOW:
      fprintf(stderr, "physical memory overflow\n");
      break;
    case E_NOT_IN_PMEM:
      fprintf(stderr, "could not find evicted page in the physical memory\n");
      break;
    case E_MRAND_3REFS:
      fprintf(stderr, "stack holding past k=3 references\n");
      break;
    case E_HASHMAP:
      fprintf(stderr, "failed to retrieve page from hashmap\n");
      break;
  }
  exit(EXIT_FAILURE);
}
