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
//TODO: remove below
#include <fcntl.h>

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

// Doubly linked list stack for LRU
node* head;
node* tail;

int main(int argc, char *argv[]) {
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
  strat_t strategy = -1;
  if (strcmp(tmpstrat, "none") == 0) {
    strategy = NONE;
  } else if (strcmp(tmpstrat, "mrand") == 0) {
    strategy = MRAND;
  } else if (strcmp(tmpstrat, "lru") == 0) {
    strategy = LRU;
  } else if (strcmp(tmpstrat, "sec") == 0) {
    strategy = SEC;
  } else {
    print_error(E_STRAT);
  }

  output = (t_output *) calloc(1, sizeof(t_output));
  struct timeval start, end;

  gettimeofday(&start, NULL);
  init(pagesize, memsize);
  simulate(pagesize, memsize, strategy);
  gettimeofday(&end, NULL);
  double elapsed = end.tv_sec - start.tv_sec;
  print_output(tmpstrat, elapsed);

  /* int extern len; */
  /* printf("################################\n"); */
  /* for (int i = 0; i < len; i++) { */
  /*   printf("entry index: %i\n", i); */
  /*   printf("v_addr: %i\n", pagetable[i]->virtual_addr); */
  /*   printf("p_addr: %i\n", pagetable[i]->physical_addr); */
  /*   printf("valid: %i\n", pagetable[i]->valid); */
  /*   printf("reference_bit: %i\n", pagetable[i]->reference_bit); */
  /*   printf("modified: %i\n", pagetable[i]->modified); */
  /*   printf("\n"); */
  /* } */
  /* printf("################################\n"); */
}

void init(int pagesize, uint32_t memsize) {
  numframes = memsize/pagesize;
  page_numbits = SYS_BITS - log2(pagesize);

  init_ptable(pow(2, page_numbits));
  memory = malloc(numframes * sizeof(uint32_t));

  head = NULL;
  tail = NULL;
}

// Main function for the simulator
void simulate(int pagesize, uint32_t memsize, strat_t strat) {
  char ref_string[SYS_BITS/8];

  // TODO: remove debug
  /* printf("page_numbits: %i\n", page_numbits); */
  int fd = open("writetest.txt", O_RDONLY);

  /* while(read(fd, ref_string, SYS_BITS/8)) { */
  while(read(STDIN_FILENO, ref_string, SYS_BITS/8)) {
    // Order of ref_string from MSB to LSB is:
    // ref[3] ref[2] ref[1] ref[0]
    printf("\nref_string: %x", ref_string[3] & 0xff);
    printf("%x", ref_string[2] & 0xff);
    printf("%x", ref_string[1] & 0xff);
    printf("%x\n", ref_string[0] & 0xff);

    output->memrefs++;
    output->pagefaults += parse_operation(ref_string, strat);
  }
}

// Parses the reference string for the operation
int parse_operation(char ref_string[], strat_t strat) {
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

  switch(oper_bits) {
    case 0:
      inc_acc(oper_byte);
      return 0;
    case 1:
      dec_acc(oper_byte);
      return 0;
    case 2:
      return write_op(pNum, strat);
    case 3:
      return read_op(pNum, strat);
    default:
      print_error(E_OP_PARSE);
      return 0;
  }
}

// Increment Accumulator operation
void inc_acc(char oper_byte) {
  int value = (oper_byte & 0x3F);
  output->acc += value;
}

// Decrement Accumulator operation
void dec_acc(char oper_byte) {
  int value = (oper_byte & 0x3F);
  output->acc -= value;
}

// Write data to page containing reference word
int write_op(uint32_t pNum, strat_t strat) {
  int returnval = 0;

  output->writes++;
  printf("pNum: %i\n", pNum);

  uint32_t refpage_idx = getEntry(pNum);
  t_ptentry* ref_page = pagetable[refpage_idx];
  ref_page->modified = 1;

  returnval = check_fault(refpage_idx, strat);

  // TODO: debug
  if (ref_page->valid == 0) {
    printf("non valid page referenced at end of write\n");
  }

  return returnval;
}

// Read data from page containing reference word
int read_op(uint32_t pNum, strat_t strat) {
  printf("pNum: %i\n", pNum);
  //TODO
  return 0;
}

int check_fault(uint32_t refpage_idx, strat_t strat) {
  int returnval = 0;
  uint32_t pNum = pagetable[refpage_idx]->virtual_addr;
  // If there is a page fault, handle
  if (check_pmem(pNum) == -1 && strat != NONE) {
    // If phys memory is full, need to evict a page
    if (pmem_len == numframes) {
      uint32_t freed = handle_pfault(strat);
      load_page(freed, refpage_idx, strat);
    }
    // if not full, just load the page in
    else if (pmem_len < numframes) {
      load_page(pmem_len, refpage_idx, strat);
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
  return returnval;
}

uint32_t check_pmem(uint32_t v_addr) {
  /* printf("len: %i\n", pmem_len); */
  /* printf("v_addr: %i\n", v_addr); */
  for (uint32_t i = 0; i < pmem_len; i++) {
    if(memory[i] == v_addr) {
      printf("----------still in mem: index: %i ---------\n", i);
      return i;
    }
  }
  return -1;
}

void evict_page(uint32_t pmem_idx, uint32_t page_idx) {
  t_ptentry* pageEvicted = pagetable[page_idx];

  /* // TODO: REMOVE DEBUG */
  /* printf("page evict info: \n"); */
  /* printf("pagetable index: %i\n", page_idx); */
  /* printf("page->v_addr: %i\n", pageEvicted->virtual_addr); */
  /* printf("page->p_addr: %i\n", pageEvicted->physical_addr); */
  /* printf("page->valid: %i\n", pageEvicted->valid); */
  /* printf("page->ref_bit: %i\n", pageEvicted->reference_bit); */
  /* printf("page->modified: %i\n", pageEvicted->modified); */

  if(pageEvicted->valid == 0) {
    printf("EVICTING A NON-VALID PAGE");
  }
  /* else { */
  /*   printf("------ EVICTING VALID PAGE -----"); */
  /* } */

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

void load_page(uint32_t avail_index, uint32_t page_idx, strat_t strat) {
  t_ptentry* page = pagetable[page_idx];

  memory[avail_index] = page->virtual_addr;
  page->physical_addr = avail_index;
  page->valid = 1;
  pmem_len++;

  // If LRU strategy, keep a stack of page numbers
  if (strat == LRU) {
    push(&head, &tail, page->virtual_addr);
  }
}

uint32_t handle_pfault(strat_t strat) {
  switch (strat) {
    case NONE:
      return none_handler();
      break;
    case MRAND:
      return mrand_handler(pmem_len);
      break;
    case LRU:
      //TODO
      printf("LRU %i\n", LRU);
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
    case 9:
      fprintf(stderr, "\n");
      break;
    case 10:
      fprintf(stderr, "\n");
      break;
  }
  exit(EXIT_FAILURE);
}
