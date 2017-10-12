/**
  *
  * a2chat.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "a2chat.h"

int main(int argc, char *argv[]) {
  char* option = argv[1];
  printf("%s\n", option); // TESTING
  if (strncmp(option, "-s", 2) == 0) {
    if (argc != 4) {
      print_error(E_USG_S);
    }
  }
  else if (strncmp(option, "-c", 2) == 0) {
    if (argc != 3) {
      print_error(E_USG_C);
    }
  } else {
    print_error(E_USG);
  }
}

// Prints errors to the user depending on the code.
void print_error(int errorcode) {
  switch (errorcode) {
    case 1:
      fprintf(stderr, "%s", "usage: a2chat -s baseName nclient\n");
      break;
    case 2:
      fprintf(stderr, "%s", "usage: a2chat -c baseName\n");
      break;
    case 3:
      fprintf(stderr, "%s", "usage: a2chat [-c | -s] baseName [nclient]\n");
      break;
  }
  exit(EXIT_FAILURE);
}
