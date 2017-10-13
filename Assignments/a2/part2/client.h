/**
  *
  * client.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 1
  */

#ifndef client_h
#define client_h

#include <stdio.h>
#include <stdlib.h>

#define MAX_COMMAND_LINE 120
#define MAX_OUT_LINE 240

void start_client(char* baseName);
void parse_input(char* input);
int open_chat(char* username);
void list_logged();
void add_receipient();
void send_chat(char* message);
void close_client();
void exit_client();

#endif
