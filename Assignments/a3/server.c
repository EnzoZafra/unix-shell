/**
  *
  * server.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 3
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "a3chat.h"
#include "client.h"
#include "server.h"

t_conn* connections;

/* Main function for the server, contains the parsing of the client messages by
   polling the inFIFOs.
*/
void start_server(int portnumber, int nclient) {
  // Add one for stdin and one for managing socket
  struct pollfd pfd[nclient + 2];
  int newsock[nclient + 2];
  char infifo[MAX_NAME];
  char outfifo[MAX_NAME + 1];
  int file_desc, rval, timeout, numpolls, fromlen, conn_idx;
  char buf[MAX_OUT_LINE];
  char* cmd;
  struct sockaddr_in sockIN, from;
  FILE* sfp[nclient + 2];

  connections = malloc(nclient * sizeof(*connections));

  printf("Chat server begins [number of clients = %d]\n", nclient);

  // Create managing socket
  int manage_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (manage_sock < 0) {
    print_error(E_SOCKET);
    exit(1);
  }

  // bind the managing socket to a name
  sockIN.sin_family= AF_INET;
  sockIN.sin_addr.s_addr= htonl(INADDR_ANY);
  sockIN.sin_port= htons(portnumber);

  if (bind(manage_sock, (struct sockaddr *) &sockIN, sizeof sockIN) < 0) {
    print_error(E_FAIL_BIND);
    exit (1);
  }

  // indicate how many connection requests can be queued
  listen (manage_sock, nclient);

  // Add master socket in nonblocking poll array
  pfd[0].fd = manage_sock;
  pfd[0].events = POLLIN;
  pfd[0].revents = 0;

  // Add STDIN file descriptor to our array
  pfd[1].fd = STDIN_FILENO;
  pfd[1].events = POLLIN;
  pfd[1].revents = 0;

  for (int i = 0; i < nclient; i++) {
    // Build our connections struct array
    memset(connections[i].username, 0, sizeof(connections[i].username));
    connections[i].connected = 0;
    connections[i].fd = -1;
    clear_receipients(i);
  }

  timeout = 0;
  numpolls = 2;

  while (1) {
    rval= poll (pfd, numpolls, timeout);
    if (rval == -1) {
      print_error(E_POLL);
    }
    else if (rval != 0) {

      // check managing socket
      if ((numpolls < nclient) && (pfd[0].revents & POLLIN)) {

        // accept new connection
        fromlen = sizeof (from);
        newsock[numpolls]= accept(manage_sock, (struct sockaddr *) &from, &fromlen);

        /* we may also want to perform STREAM I/O */
        if ((sfp[numpolls] = fdopen(newsock[numpolls], "r")) < 0) {
          print_error(E_OPENSOCK);
          exit (1);
        }

        pfd[numpolls].fd= newsock[numpolls];
        pfd[numpolls].events= POLLIN;
        pfd[numpolls].revents= 0;

        conn_idx = numpolls - 2;
        connections[conn_idx].connected = true;
        connections[conn_idx].fd = newsock[numpolls];

        numpolls++;
      }

      // Find the fd that has data
      for (int j = 1; j <= nclient + 1; j++) {
        if (pfd[j].revents & POLLIN) {
          // Clear the buffer
          memset(buf, 0, sizeof(buf));
          if (read(pfd[j].fd, buf, MAX_OUT_LINE) > 0) {
            cmd = strtok(buf, "\n");
            if (cmd == NULL) {
              continue;
            }
            // if j == 1, this is the stdin file descriptor
            if (j == 1) {
              // Server "exit" command from STDIN terminates server
              if (strcmp(cmd, "exit") == 0) {
                close_allfd(pfd, NMAX + 2);
                free(connections);
                exit(EXIT_SUCCESS);
              }
            }
            // a client sent a command
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

/* Parses the command sent by the client */
void parse_cmd(char* buf, int pipenumber) {
  char* args;
  char* cmd = strtok(buf, "|\n");
  int index = pipenumber - 1;

  if (strcmp(cmd, "open") == 0) {
    args = strtok(NULL, "| \n");
    if (args == NULL) {
      printf("username not found. Please try again");
      return;
    }
    server_open(index, args);
  }
  else if (strcmp(cmd, "who") == 0) {
    server_list_logged(index);
  }
  else if (strcmp(cmd, "to") == 0) {
    args = strtok(NULL, "\n");
    server_add_receipient(index, args);
  }
  else if (strcmp(cmd, "<") == 0) {
    args = strtok(NULL, "\n");
    server_receive_msg(index, args);
  }
  else if (strcmp(cmd, "close") == 0) {
    server_close_client(index);
  }
  else if (strcmp(cmd, "exit") == 0) {
    server_exit_client(index);
  }
}

/* Server opens a chat session for a user. */
int server_open(int index, char* username) {
  char msg_out[MAX_BUF];
  if (username_taken(username)) {
    snprintf(msg_out, sizeof(msg_out), "[server] Error: username is already taken.\n");
    if(write(connections[index].fd, msg_out, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_OUT);
    }
    close_connection(index);
    return -1;
  }
  else {
    // Successfully locked and connected to a FIFO
    strcpy(connections[index].username, username);

    /* // Write server msg to fifo */
    write_connected_msg(username, index);
    return connections[index].fd;
  }
}

/* Server returns a list of logged users to the client */
void server_list_logged(int index) {
  char msg_out[MAX_BUF];
  char buf[MAX_BUF];

  // Build message
  memset(msg_out, 0, sizeof(msg_out));
  // Write server msg to fifo
  snprintf(msg_out, sizeof(msg_out), "[server] Current users:");
  for (int i = 0; i < NMAX; i++) {
    if (strlen(connections[i].username) != 0) {
      memset(buf, 0, sizeof(buf));
      snprintf(buf, sizeof(buf), " [%i] %s", i+1, connections[i].username);
      strcat(msg_out, buf);
    }
  }
  strcat(msg_out, "\n");
  if(write(connections[index].fd, msg_out, MAX_OUT_LINE) == -1) {
    print_error(E_WRITE_OUT);
  }
}

void server_add_receipient(int index, char* receipients) {
  char* tok = strtok(receipients, " ");
  while (tok != NULL) {
    // Increment num of receipients and add to the list
    connections[index].num_receipients++;
    strcpy(connections[index].receipients[connections[index].num_receipients - 1], tok);
    tok = strtok(NULL, " \n");
  }
}

void server_receive_msg(int index, char* msg) {
  char msg_out[MAX_BUF];

  // Build message
  memset(msg_out, 0, sizeof(msg_out));
  // Write server msg to fifo
  snprintf(msg_out, sizeof(msg_out), "[%s] %s\n", connections[index].username, msg);

  // Loop through the list of receipients
  for (int i = 0; i < connections[index].num_receipients ; i++) {
    char* receipient = connections[index].receipients[i];
    for (int j = 0; j < NMAX; j++) {
      if (strcmp(receipient, connections[j].username) == 0) {
        if(write(connections[j].fd, msg_out, MAX_OUT_LINE) == -1) {
          print_error(E_WRITE_OUT);
        }
      }
    }
  }
}

/* Server closes the chat session that is initialized by the client */
void server_close_client(int index) {
  char msg_out[MAX_BUF];

  int fd = connections[index].fd;
  memset(msg_out, 0, sizeof(msg_out));
  // Write server msg to fifo
  snprintf(msg_out, sizeof(msg_out), "[server] done\n");

  if(write(fd, msg_out, MAX_OUT_LINE) == -1) {
    print_error(E_WRITE_OUT);
  }

  close_connection(index);
}

void server_exit_client(int index) {
  close_connection(index);
}

/* Helper function to close all fds that is connected */
void close_allfd(struct pollfd pfd[], int len) {
  for (int i = 2; i < len; i++) {
    close(pfd[i].fd);
    close(connections[i-2].fd);
  }
}

/* Helper function that checks if a username supplied by the client is taken */
bool username_taken(char* username) {
  for (int i = 0; i < NMAX; i++) {
    if (strcmp(connections[i].username, username)==0) {
      return true;
    }
  }
  return false;
}

// Clear receipient list for a connection
void clear_receipients(int index) {
  for(int i = 0; i < MAX_RECEIPIENTS; i++) {
    memset(connections[index].receipients[i], 0, sizeof connections[index].receipients[i]);
  }
  connections[index].num_receipients = 0;
}

/* Helper function to close the connection */
void close_connection(int index) {
  close(connections[index].fd);
  // Successfully closed and unlocked fifo
  connections[index].connected = false;
  memset(connections[index].username, 0, sizeof(connections[index].username));
  connections[index].fd = -1;
  clear_receipients(index);
}

/* Writes a message to all of the connected users */
void write_connected_msg(char* username, int index) {
  char msg_out[MAX_BUF];
  memset(msg_out, 0, sizeof msg_out);

  // Write server msg to fifo
  snprintf(msg_out, sizeof(msg_out), "[server] User `%s` connected on FIFO %d\n",
            username, index + 1);

  for (int i = 0; i < NMAX; i++) {
    if (connections[i].connected) {
      if(write(connections[i].fd, msg_out, MAX_OUT_LINE) == -1) {
        print_error(E_WRITE_OUT);
      }
    }
  }
}
