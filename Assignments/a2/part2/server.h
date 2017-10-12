/**
  *
  * server.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 1
  */

#ifndef server_h
#define server_h

#include <stdio.h>
#include <stdlib.h>

#define MAX_FIFO_NAME 30

void start_server(char* baseName, int nclient);
void createFIFOs(char* baseName, int nclient);
#endif
