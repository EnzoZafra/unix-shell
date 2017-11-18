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
#include <sys/time.h>
#include <math.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "a3chat.h"
#include "client.h"
#include "server.h"

t_conn* connections;
struct pollfd pfd[NMAX + 2];
int numpolls;

/* Main function for the server, contains the parsing of the client messages by
   polling the socket FDs.
*/
void start_server(int portnumber, int nclient) {
  // Add one for stdin and one for managing socket
  int rval, timeout, conn_idx;
  char buf[MAX_OUT_LINE];
  char* cmd;
  struct sockaddr_in sockIN, from;
  socklen_t fromlen;

  connections = malloc(nclient * sizeof(*connections));

  printf("Chat server begins [port = %i] [nclient = %d]\n", portnumber, nclient);

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
    connections[i].kam_misses = 0;
    connections[i].checked_kam = true;
    clear_receipients(i);
  }

  timeout = 0;
  numpolls = 2;

  while (1) {
    check_kam(nclient);
    for (int i = 0; i < nclient; i++) {
      if (connections[i].kam_misses == KAL_COUNT) {
        printf("%s crashed so we closed him\n", connections[i].username);
        close_connection(i);
      }
    }

    rval= poll(pfd, numpolls, timeout);
    if (rval == -1) {
      print_error(E_POLL);
    }
    else if (rval != 0) {

      // check managing socket
      // nclient + 3 because: 5 max clients + stdin + managing socket + 1 extra for queued client
      if ((numpolls <= nclient + 3) && (pfd[0].revents & POLLIN)) {

        // accept new connection
        fromlen = sizeof (from);

        // server is already serving max num of clients, let it connect so we can give it an error.
        if (numpolls == nclient + 2) {
          int errorfd = accept(manage_sock, (struct sockaddr *) &from, &fromlen);
          char msg_out[MAX_BUF];
          snprintf(msg_out, sizeof(msg_out), "[server] Error: server is currently full. Try again later\n");
          if(write(errorfd, msg_out, MAX_OUT_LINE) == -1) {
            print_error(E_WRITE);
          }
          shutdown(errorfd, SHUT_WR);
        }
        else {

          conn_idx = numpolls - 2;
          connections[conn_idx].fd = accept(manage_sock, (struct sockaddr *) &from, &fromlen);
          connections[conn_idx].connected = true;
          gettimeofday(&connections[conn_idx].start, NULL);
          pfd[numpolls].fd= connections[conn_idx].fd;
          pfd[numpolls].events= POLLIN;
          pfd[numpolls].revents= 0;
          numpolls++;
        }
      }

      // Find the fd that has data
      for (int j = 1; j < numpolls; j++) {
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
            // TODO: not sure what to do here yet.
            /* print_error(E_READ); */
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
  int index = pipenumber - 2;
  char ack[KAL_LENGTH + 1];

  snprintf(ack, sizeof(ack), "%c%c%c%c%c", KAL_CHAR, KAL_CHAR, KAL_CHAR, KAL_CHAR, KAL_CHAR);
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
  // TODO: Change this to keepalive msg
  else if (strcmp(cmd, ack) == 0) {
    printf("Got an ack\n");
    connections[index].kam_misses = 0;
  }
}

/* Server opens a chat session for a user. */
int server_open(int index, char* username) {
  char msg_out[MAX_BUF];
  if (username_taken(username)) {
    snprintf(msg_out, sizeof(msg_out), "[server] Error: username is already taken.\n");
    if(write(connections[index].fd, msg_out, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE);
    }
    close_connection(index);
    return -1;
  }
  else {
    // Username is not taken
    strcpy(connections[index].username, username);

    /* // Write server msg to socket */
    write_connected_msg(username);
    return connections[index].fd;
  }
}

/* Server returns a list of logged users to the client */
void server_list_logged(int index) {
  char msg_out[MAX_BUF];
  char buf[MAX_BUF];

  // Build message
  memset(msg_out, 0, sizeof(msg_out));
  // Write server msg to socket
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
    print_error(E_WRITE);
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
  snprintf(msg_out, sizeof(msg_out), "[%s] %s\n", connections[index].username, msg);

  // Loop through the list of receipients
  for (int i = 0; i < connections[index].num_receipients ; i++) {
    char* receipient = connections[index].receipients[i];
    for (int j = 0; j < NMAX; j++) {
      if (strcmp(receipient, connections[j].username) == 0) {
        if(write(connections[j].fd, msg_out, MAX_OUT_LINE) == -1) {
          print_error(E_WRITE);
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
  // Write server msg to socket
  snprintf(msg_out, sizeof(msg_out), "[server] done\n");

  if(write(fd, msg_out, MAX_OUT_LINE) == -1) {
    print_error(E_WRITE);
  }

  close_connection(index);
}

void server_exit_client(int index) {
  close_connection(index);
}

/* Helper function to close all fds that is connected */
void close_allfd(struct pollfd pfd[], int len) {
  for (int i = 2; i < len; i++) {
    shutdown(pfd[i].fd, SHUT_WR);
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
  numpolls--;
  shutdown(connections[index].fd, SHUT_WR);
  connections[index].connected = false;
  memset(connections[index].username, 0, sizeof(connections[index].username));
  connections[index].fd = -1;
  connections[index].kam_misses = 0;
  connections[index].checked_kam = true;
  pfd[index+2].fd = -1;
  clear_receipients(index);
  pollfd_conn_defrag(pfd, connections, NMAX + 2, NMAX);
}

/* Writes a message to all of the connected users */
void write_connected_msg(char* username) {
  char msg_out[MAX_BUF];
  memset(msg_out, 0, sizeof msg_out);

  // Write server msg to all sockets
  snprintf(msg_out, sizeof(msg_out), "[server] User `%s` logged in.\n", username);

  for (int i = 0; i < NMAX; i++) {
    if (connections[i].connected) {
      if(write(connections[i].fd, msg_out, MAX_OUT_LINE) == -1) {
        print_error(E_WRITE);
      }
    }
  }
}

void pollfd_conn_defrag(struct pollfd *pfd, t_conn *conn, int pfd_size, int conn_size) {
    int new_size = 0;
    for (int x = 0; x < pfd_size; x++) {
      if (pfd[x].fd < 0) {
        continue;
      }
      pfd[new_size++] = pfd[x];
    }
    new_size = 0;
    for (int y = 0; y < conn_size; y++) {
      if (conn[y].fd < 0) {
        continue;
      }
      conn[new_size++] = conn[y];
    }
}

void check_kam(int connections_size) {
  struct timeval curr;
  double elapsed;

  for(int i = 0; i < connections_size; i++) {
    if(connections[i].connected) {
      gettimeofday(&curr, NULL);
      elapsed = curr.tv_sec - connections[i].start.tv_sec;
      if (fmod(elapsed, (double) KAL_INTERVAL) == 0) {
        if (!connections[i].checked_kam) {
          connections[i].kam_misses++;
          connections[i].checked_kam = true;
        }
      } else {
        connections[i].checked_kam = false;
      }
    }
  }
}
