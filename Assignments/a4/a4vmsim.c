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
#include "a4vmsim.h"

t_output *output;

int main(int argc, char *argv[]) {
  if (argc < 4 || argc > 4) {
    print_error(E_USG);
  }

  int pagesize = atoi(argv[1]);
  if (pagesize < 256 || pagesize > 8192 || !ispowerof2(pagesize)) {
    print_error(E_PAGESIZE);
  }

  int memsize = atoi(argv[2]);
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
  simulate(pagesize, memsize, strategy);
  gettimeofday(&end, NULL);
  double elapsed = end.tv_sec - start.tv_sec;
  print_output(tmpstrat, elapsed);
}

// Main function for the simulator
void simulate(int pagesize, int memsize, strat_t strat) {
  int numframes = memsize/pagesize;
  char ref_string[SYS_BITS/8];
  oper_t operation = -1;

  int page_numbits = SYS_BITS - log2(pagesize);

  while(read(STDIN_FILENO, ref_string, SYS_BITS/8)) {
    // Order of ref_string from MSB to LSB is:
    // ref[3] ref[2] ref[1] ref[0]
    printf("ref_string: %x", ref_string[3] & 0xff);
    printf("%x", ref_string[2] & 0xff);
    printf("%x", ref_string[1] & 0xff);
    printf("%x\n", ref_string[0] & 0xff);
    // do things here
    output->memrefs++;
    operation = parse_operation(ref_string);
  }
}

// Parses the reference string for the operation
oper_t parse_operation(char ref_string[]) {
  char oper_byte = ref_string[0];
  // Shift 6 bits to the right since the opcode
  // resides in the 2 most significant bits
  char oper_bits = (oper_byte & 0xff) >> 6;
  switch(oper_bits) {
    case 0:
      inc_acc(oper_byte);
      return INC_ACC;
      break;
    case 1:
      dec_acc(oper_byte);
      return DEC_ACC;
      break;
    case 2:
      write_op();
      return WRITE;
      break;
    case 3:
      read_op();
      return READ;
      break;
    default:
      print_error(E_OP_PARSE);
      return -1;
  }
}

// Increment Accumulator operation
void inc_acc(char oper_byte) {
  int value = (oper_byte & 0x3F);
  output->acc += value;
  //TODO
}

// Decrement Accumulator operation
void dec_acc(char oper_byte) {
  int value = (oper_byte & 0x3F);
  output->acc -= value;
  //TODO
}

// Write data to page containing reference word
void write_op() {
  output->writes++;
  //TODO
}

// Read data from page containing reference word
void read_op() {
  //TODO
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
bool ispowerof2(unsigned int x) {
   return x && !(x & (x - 1));
}

// Rounds up the value to the nearest multiple of 'multipleof'
int roundNearMult(int value, int multipleof) {
  int tmp = value % multipleof;
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
    case 6:
      fprintf(stderr, "\n");
      break;
    case 7:
      fprintf(stderr, "\n");
      break;
    case 8:
      fprintf(stderr, "\n");
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
