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
  struct pollfd fds[NMAX];
  char infifo[MAX_FIFO_NAME];
  int file_desc, rval, timeout, len;
  char buf[MAX_OUT_LINE];
  char* username;
  char* cmd;
  char* fifonum;

  createFIFOs(baseName, nclient);
  printf("Chat server begins [number of clients = %d]\n", nclient);

  for (int i = 0; i < nclient; i++) {
    memset(infifo, 0, sizeof infifo);
    snprintf(infifo, sizeof infifo, "%s-%d.in", baseName, i+1);
    file_desc = open(infifo, O_RDONLY | O_NONBLOCK);

    fds[i].fd = file_desc;
    fds[i].events = POLLIN;
    fds[i].revents = 0;
    printf("infifo: %s    ; file_desc: %i\n", infifo, fds[i].fd); // Testing
  }
  timeout = 0;

  while (1) {
    rval = poll(fds, nclient, timeout);
    if (rval == -1) {
      print_error(E_POLL);
    }
    else if (rval != 0) {
      for (int j = 0; j < nclient; j++) {
        if (fds[j].revents & POLLIN) {
          len = read(fds[j].fd, buf, MAX_OUT_LINE);
          printf("received: %s\n", buf);
          username = strtok(buf, ", \n");
          cmd = strtok(NULL, ", \n");
          fifonum = strtok(NULL, ", \n");
          printf("username: %s ;;; cmd: %s ;;; fifonum: %s\n", username, cmd, fifonum);

        }
      }

    }
  }
}

void createFIFOs(char* baseName, int nclient) {
  char in_name[MAX_FIFO_NAME];
  char out_name[MAX_FIFO_NAME + 1];
  char command[50];

  for (int i = 1; i <= nclient; i++) {
    snprintf(in_name, sizeof(in_name), "%s-%d.in", baseName, i);
    snprintf(out_name, sizeof(out_name), "%s-%d.out", baseName, i);

    if (mkfifo(in_name, S_IRWXU) != 0 || mkfifo(out_name, S_IRWXU) != 0) {
      /* print_error(E_FIFO); */ // Testing  uncomment later
    }
  }
}
