/**
  *
  * a3chat.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 3
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "a3chat.h"
#include "client.h"
#include "server.h"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    print_error(E_USG);
  }

  char* option = argv[1];
  int portNum = atoi(argv[2]);
  if (portNum <= 0) {
    print_error(E_INVALID_PORTNUM);
  }

  if (strncmp(option, "-s", 2) == 0) {
    if (argc != 4) {
      print_error(E_USG_S);
    }
    int nclient = atoi(argv[3]);
    if (nclient <= 0 || nclient > NMAX) {
      print_error(E_NEGATIVE);
    }
    start_server(portNum, nclient);
  }
  else if (strncmp(option, "-c", 2) == 0) {
    if (argc != 4) {
      print_error(E_USG_C);
    }
    char* serverAddress = argv[3];
    start_client(portNum, serverAddress);
  }
  else {
    print_error(E_USG);
  }
}

// Prints errors to the user depending on the code.
void print_error(int errorcode) {
  switch (errorcode) {
    case 1:
      fprintf(stderr, "usage: a3chat -s portnumber nclient\n");
      break;
    case 2:
      fprintf(stderr, "usage: a3chat -c portnumber serveraddress\n");
      break;
    case 3:
      fprintf(stderr, "usage: a3chat [-c | -s] portnumber [serveraddress | nclient]\n");
      break;
    case 4:
      fprintf(stderr, "nclient must be an integer in the range 0 < nclient <= 5\n");
      break;
    case 5:
      fprintf(stderr, "failed to create socket\n");
      break;
    case 6:
      fprintf(stderr, "failed to write to socket. client may have crashed\n");
      break;
    case 7:
      fprintf(stderr, "error when polling file descriptors. errno: %i\n", errno);
      break;
    case 8:
      fprintf(stderr, "time buffer in print_report too small\n");
      break;
    case 9:
      fprintf(stderr, "failed to read socket errno: %i\n", errno);
      break;
    case 10:
      fprintf(stderr, "failed to bind server managing socket\n");
      break;
    case 11:
      fprintf(stderr, "invalid portnumber\n");
      break;
    case 12:
      fprintf(stderr, "failed to open FILE for client socket\n");
      break;
    case 13:
      fprintf(stderr, "failed to get host by name\n");
      break;
    case 14:
      fprintf(stderr, "failed to connect to the server\n");
      break;
    case 15:
      fprintf(stderr, "failed to apply signal handler to SIGALRM\n");
      break;
    case 16:
      fprintf(stderr, "pfd/conn index conversion failed\n");
      break;
  }
  exit(EXIT_FAILURE);
}
