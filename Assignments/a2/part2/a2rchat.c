/**
  *
  * a2rchat.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 1
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "a2rchat.h"
#include "client.h"
#include "server.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    print_error(E_USG);
  }

  char* option = argv[1];
  char* baseName = argv[2];

  if (strncmp(option, "-s", 2) == 0) {
    if (argc != 4) {
      print_error(E_USG_S);
    }
    int nclient = atoi(argv[3]);
    if (nclient <= 0 || nclient > NMAX) {
      print_error(E_NEGATIVE);
    }
    start_server(baseName, nclient);
  }
  else if (strncmp(option, "-c", 2) == 0) {
    if (argc != 3) {
      print_error(E_USG_C);
    }
    start_client(baseName);
  }
  else {
    print_error(E_USG);
  }
}

// Prints errors to the user depending on the code.
void print_error(int errorcode) {
  switch (errorcode) {
    case 1:
      fprintf(stderr, "usage: a2chat -s baseName nclient\n");
      break;
    case 2:
      fprintf(stderr, "usage: a2chat -c baseName\n");
      break;
    case 3:
      fprintf(stderr, "usage: a2chat [-c | -s] baseName [nclient]\n");
      break;
    case 4:
      fprintf(stderr, "client must be an integer in the range 0 < nclient <= 5\n");
    case 5:
      fprintf(stderr, "failed to make FIFOs. Please delete all "
                      "FIFOs with the following command:\nfind . -type p -delete\n");
    case 6:
      fprintf(stderr, "failed to write to inFIFO\n");
    case 7:
      fprintf(stderr, "error when polling file descriptors\n");
    case 8:
      fprintf(stderr, "failed to write to outFIFO\n");
    case 9:
      fprintf(stderr, "failed to write read FIFO\n");
    case 10:
      fprintf(stderr, "failed to connect to corresponding outFIFO\n");

  }
  exit(EXIT_FAILURE);
}
