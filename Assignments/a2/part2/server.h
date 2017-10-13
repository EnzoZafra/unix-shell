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
void parse_cmd(char*);
void server_open();
void server_list_logged();
void server_add_receipient();
void server_receive_msg();
void server_close_client();
void server_exit_client();
void createFIFOs(char* baseName, int nclient);
#endif
