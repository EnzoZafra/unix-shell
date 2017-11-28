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

#include "stack.h"
#include "a4vmsim.h"

int main(int argc, char *argv[]) {
  if (argc < 4 || argc > 4) {
    print_error(E_USG);
  }

  int pagesize = atoi(argv[1]);
  if (pagesize <= 0 || !ispowerof2(pagesize)) {
    print_error(E_PAGESIZE);
  }

  int memsize = atoi(argv[2]);
  if (memsize <= 0) {
    print_error(E_MEMSIZE);
  }
  memsize = roundNearMult(memsize, pagesize);

  char* strategy = argv[3];
  printf("pagesize: %i, memsize: %i, strat: %s\n", pagesize, memsize, strategy);
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
    case 4:
      fprintf(stderr, "\n");
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
