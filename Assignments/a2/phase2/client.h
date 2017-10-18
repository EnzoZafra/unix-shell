/**
  *
  * client.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 2
  */

#ifndef client_h
#define client_h

#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF 512
#define MAX_OUT_LINE 240

void start_client(char* baseName);
void parse_input(char* input);
int open_chat(char* username);
void list_logged();
void add_receipient(char* receipients);
void send_chat(char* message);
void close_client();
void exit_client();

#endif
