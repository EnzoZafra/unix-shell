/**
  *
  * a2chat.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 2
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "a2chat.h"
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
      fprintf(stderr, "failed to write to inFIFO! errno: %i\n", errno);
    case 7:
      fprintf(stderr, "error when polling file descriptors. errno: %i\n", errno);
    case 8:
      fprintf(stderr, "failed to write to outFIFO. errno: %i\n", errno);
    case 9:
      fprintf(stderr, "failed to write read FIFO. errno: %i\n", errno);
    case 10:
      fprintf(stderr, "failed to connect to corresponding outFIFO. errno: %i\n", errno);
  }
  exit(EXIT_FAILURE);
}
