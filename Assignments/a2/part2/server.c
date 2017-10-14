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

char* baseFifoName;
t_conn* connections;

void start_server(char* baseName, int nclient) {
  // Add one for stdin
  struct pollfd in_fds[NMAX + 1];
  char infifo[MAX_NAME];
  char outfifo[MAX_NAME + 1];
  int file_desc, rval, timeout, len;
  char buf[MAX_OUT_LINE];
  char* cmd;

  connections = malloc(NMAX * sizeof(*connections));

  baseFifoName = baseName;

  createFIFOs(baseName, nclient);
  printf("Chat server begins [number of clients = %d]\n", nclient);

  // Add STDIN file descriptor to our array
  in_fds[0].fd = STDIN_FILENO;
  in_fds[0].events = POLLIN;
  in_fds[0].revents = 0;

  for (int i = 0; i < nclient; i++) {
    // Build our connections struct array
    memset(outfifo, 0, sizeof outfifo);
    snprintf(outfifo, sizeof outfifo, "%s-%d.out", baseName, i+1);
    strcpy(connections[i].outfifo, outfifo);
    memset(connections[i].username, 0, sizeof(connections[i].username));
    connections[i].connected = 0;
    connections[i].fd = -1;

    // Build in_fds struct array
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
            printf("received: %s\n", buf); // Testing
            cmd = strtok(buf, "\n");
            if (cmd == NULL) {
              continue;
            }
            printf("cmd: %s\n", cmd);
            // if j == 0, this is the stdin file descriptor
            if (j == 0) {
              // Server "exit" command from STDIN terminates server
              if (strcmp(cmd, "exit") == 0) {
                free(connections);
                exit(EXIT_SUCCESS);
              }
            }
            // pipe FD
            else {
              parse_cmd(cmd, j);
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

void parse_cmd(char* buf, int pipenumber) {
  char* cmd = strtok(buf, "|\n");
  if (strcmp(cmd, "open") == 0) {
    char* username = strtok(NULL, "|\n");
    server_open(pipenumber, username);
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

int server_open(int pipenumber, char* username) {
  int file_desc;
  char msg_out[MAX_BUF];

  char *outfifo = connections[pipenumber-1].outfifo;

  file_desc = open(outfifo, O_RDWR | O_NONBLOCK);

  if (lockf(file_desc, F_TEST, 0) == -1) {
    close(file_desc);
    print_error(E_CONN_OUTFIFO);
  }
  else {
    if (lockf(file_desc, F_LOCK, 0) != -1) {
      // Successfully locked and connected to a FIFO
      connections[pipenumber - 1].connected = true;
      strcpy(connections[pipenumber - 1].username, username);
      connections[pipenumber - 1].fd = file_desc;

      memset(msg_out, 0, sizeof(msg_out));
      // Write server msg to fifo
      snprintf(msg_out, sizeof(msg_out), "[server] User `%s` connected on FIFO %d\n",
                username, pipenumber);

      if(write(file_desc, msg_out, MAX_OUT_LINE) == -1) {
        print_error(E_WRITE_OUT);
      }

      return file_desc;
    }
  }
  return -1;
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

}

void server_exit_client() {
  //TODO:
  printf("Not implemented yet\n");
}


void createFIFOs(char* baseName, int nclient) {
  char in_name[MAX_NAME];
  char out_name[MAX_NAME + 1];

  for (int i = 1; i <= nclient; i++) {
    snprintf(in_name, sizeof(in_name), "%s-%d.in", baseName, i);
    snprintf(out_name, sizeof(out_name), "%s-%d.out", baseName, i);

    if (mkfifo(in_name, S_IRWXU) != 0 || mkfifo(out_name, S_IRWXU) != 0) {
      /* print_error(E_FIFO); */ // Testing  uncomment later
    }
  }
}
