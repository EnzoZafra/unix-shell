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
#include <stdbool.h>

#define MAX_FIFO_NAME 30

typedef struct conn {
   char *fifoname;
   int fd;
   bool connected;
   char *username;
} t_conn;

void start_server(char* baseName, int nclient);
void parse_cmd(char* cmd, int pipenumber);
int server_open(int pipenumber, char* username);
void server_list_logged();
void server_add_receipient();
void server_receive_msg();
void server_close_client();
void server_exit_client();
void createFIFOs(char* baseName, int nclient);
#endif
