/**
  *
  * server.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 2
  */

#ifndef server_h
#define server_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <poll.h>

#define MAX_NAME 30
#define MAX_RECEIPIENTS 10

typedef struct conn {
  char outfifo[MAX_NAME + 1];
  int fd;
  bool connected;
  char username[MAX_NAME];
  char receipients[MAX_RECEIPIENTS][MAX_NAME];
  int num_receipients;
} t_conn;

void start_server(char* baseName, int nclient);
void parse_cmd(char* cmd, int index);
int server_open(int index, char* username);
void server_list_logged(int index);
void server_add_receipient(int index, char* receipients);
void server_receive_msg(int index, char* msg);
void server_close_client(int index);
void server_exit_client(int index);
void createFIFOs(char* baseName, int nclient);
void close_allfd(struct pollfd in_fds[], int len);
void clear_receipients(int index);
#endif
