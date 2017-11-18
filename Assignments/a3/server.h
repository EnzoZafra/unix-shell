/**
  *
  * server.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 3
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
  int fd;
  bool connected;
  char username[MAX_NAME];
  char receipients[MAX_RECEIPIENTS][MAX_NAME];
  int num_receipients;
  int kam_misses;
  bool checked_kam;
  struct timeval start;
} t_conn;

void start_server(int portnumber, int nclient);
void parse_cmd(char* cmd, int index);
void server_open(int index, char* username);
void server_list_logged(int index);
void server_add_receipient(int index, char* receipients);
void server_receive_msg(int index, char* msg);
void server_close_client(int index);
void server_exit_client(int index);
void close_allfd(struct pollfd in_fds[], int len);
void clear_receipients(int index);
void close_connection(int index);
bool username_taken(char* username);
void write_connected_msg(char* username);
void pollfd_conn_defrag(struct pollfd *pfd, t_conn *conn, int pfd_size, int conn_size);
void check_kam(int connections_size);
void free_connection(int index);

#endif
