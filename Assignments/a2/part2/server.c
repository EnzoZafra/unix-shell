/**
  *
  * server.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 1
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>

#include "a2rchat.h"
#include "client.h"
#include "server.h"

void start_server(char* baseName, int nclient) {
  // Add one for stdin
  struct pollfd in_fds[NMAX + 1];
  struct pollfd out_fds[NMAX];
  char infifo[MAX_FIFO_NAME];
  int file_desc, rval, timeout, len;
  char buf[MAX_OUT_LINE];
  char* username;
  char* cmd;
  char* fifonum;

  createFIFOs(baseName, nclient);
  printf("Chat server begins [number of clients = %d]\n", nclient);

  // Add STDIN file descriptor to our array
  in_fds[0].fd = STDIN_FILENO;
  in_fds[0].events = POLLIN;
  in_fds[0].revents = 0;

  for (int i = 0; i < nclient; i++) {
    memset(infifo, 0, sizeof infifo);
    snprintf(infifo, sizeof infifo, "%s-%d.in", baseName, i+1);
    file_desc = open(infifo, O_RDONLY | O_NONBLOCK);

    in_fds[i+1].fd = file_desc;
    in_fds[i+1].events = POLLIN;
    in_fds[i+1].revents = 0;
    printf("infifo: %s ; file_desc: %i\n", infifo, in_fds[i+1].fd); // Testing
  }
  timeout = 0;

  while (1) {
    rval = poll(in_fds, nclient, timeout);
    if (rval == -1) {
      print_error(E_POLL);
    }
    // If there is a return value in poll then a pipe has data
    else if (rval != 0) {
      // Find the fd that has data
      for (int j = 0; j <= nclient; j++) {
        if (in_fds[j].revents & POLLIN) {
          // Clear the buffer
          memset(buf, 0, sizeof(buf));
          if (read(in_fds[j].fd, buf, MAX_OUT_LINE) > 0) {
            printf("received: %s\n", buf);
            cmd = strtok(buf, ",\n");
            printf("cmd: %s\n", cmd);
            // if j == 0, this is the stdin file descriptor
            if (j == 0) {
              // Server "exit" command from STDIN terminates server
              if (strcmp(cmd, "exit") == 0) {
                exit(EXIT_SUCCESS);
              }
            }
            // pipe FD
            else {
              parse_cmd(cmd);
            }
          }
          else {
            print_error(E_READ);
          }
        }
      }
    }
  }
}



void parse_cmd(char* cmd) {
  if (strcmp(cmd, "open") == 0) {
    server_open();
  }
  else if (strcmp(cmd, "who") == 0) {
    // Not needed for phase 1
    /* server_list_logged(); */
    return;
  }
  else if (strcmp(cmd, "to") == 0) {
    // Not needed for phase 1
    /* server_add_receipient(); */
    return;
  }
  else if (strcmp(cmd, "<") == 0) {
    server_receive_msg();
  }
  else if (strcmp(cmd, "close") == 0) {
    server_close_client();
  }
  else if (strcmp(cmd, "exit") == 0) {
    server_exit_client();
  }
}

void server_open() {
  //TODO:
  printf("Not implemented yet\n");
}

void server_list_logged() {
  //TODO:
  printf("Not implemented yet\n");
}

void server_add_receipient() {
  //TODO:
  printf("Not implemented yet\n");
}

void server_receive_msg() {
  //TODO:
  printf("Not implemented yet\n");
}

void server_close_client() {
  //TODO:
  printf("Not implemented yet\n");
}

void server_exit_client() {
  //TODO:
  printf("Not implemented yet\n");
}


void createFIFOs(char* baseName, int nclient) {
  char in_name[MAX_FIFO_NAME];
  char out_name[MAX_FIFO_NAME + 1];

  for (int i = 1; i <= nclient; i++) {
    snprintf(in_name, sizeof(in_name), "%s-%d.in", baseName, i);
    snprintf(out_name, sizeof(out_name), "%s-%d.out", baseName, i);

    if (mkfifo(in_name, S_IRWXU) != 0 || mkfifo(out_name, S_IRWXU) != 0) {
      /* print_error(E_FIFO); */ // Testing  uncomment later
    }
  }
}
