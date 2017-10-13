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
#

#include "a2rchat.h"
#include "client.h"
#include "server.h"

void start_server(char* baseName, int nclient) {
  struct pollfd fds[NMAX];
  char infifo[MAX_FIFO_NAME];
  int file_desc, rval, timeout;

  createFIFOs(baseName, nclient);
  printf("Chat server begins [number of clients = %d]\n", nclient);

  for (int i = 1; i <= nclient; i++) {
    memset(infifo, 0, sizeof infifo);
    snprintf(infifo, sizeof infifo, "%s-%d.in", baseName, i);
    file_desc = open(infifo, O_RDONLY | O_NONBLOCK);

    fds[i].fd = file_desc;
    fds[i].events = POLLIN;
    fds[i].revents = 0;
  }
  timeout = 2000;

  while (1) {
    /* for (int i = 1; i <= nclient; i++) { */
    /*   memset(infifo, 0, sizeof infifo); */
    /*   snprintf(infifo, sizeof infifo, "%s-%d.in", baseName, i); */
    /*   file_desc = open(infifo, O_RDONLY | O_NONBLOCK); */

    /*   fds[i].fd = file_desc; */
    /*   fds[i].events = POLLIN; */
    /*   fds[i].revents = 0; */
    /* } */
    rval = poll(fds, nclient, timeout);
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
      print_error(E_FIFO);
    }

    // OSX TEST
    snprintf(command, sizeof(command), "chmod 666 %s", in_name);
    system(command);
    snprintf(command, sizeof(command), "chmod 666 %s", out_name);
    system(command);

  }
}
