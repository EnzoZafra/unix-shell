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

  int page_numbits = SYS_BITS - log2(pagesize);

  while(read(0, ref_string, SYS_BITS/8)) {
    // do things here
    output->memrefs++;
  }
}

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
    case 5:
      fprintf(stderr, "\n");
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
