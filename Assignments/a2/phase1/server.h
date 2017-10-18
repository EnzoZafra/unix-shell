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
#include <poll.h>

#define MAX_NAME 30

typedef struct conn {
   char outfifo[MAX_NAME + 1];
   int fd;
   bool connected;
   char username[MAX_NAME];
} t_conn;

void start_server(char* baseName, int nclient);
void parse_cmd(char* cmd, int pipenumber);
int server_open(int pipenumber, char* username);
void server_list_logged();
void server_add_receipient();
void server_receive_msg(int pipenumber, char* msg);
void server_close_client(int pipenumber);
void server_exit_client(int pipenumber);
void createFIFOs(char* baseName, int nclient);
void close_allfd(struct pollfd in_fds[], int len);
#endif
