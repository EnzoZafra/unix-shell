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
#include <signal.h>
#include <errno.h>

#include "a3chat.h"
#include "client.h"
#include "server.h"

t_conn* connections;
// extra one for managing socket and another extra for STDIN
struct pollfd pfd[NMAX + 2];
int nclient, crashcount, numpolls;
bool alarmset = false;
char crashes[MAX_CRASHES][MAX_BUF];

/* Main function for the server, contains the parsing of the client messages by
   polling the socket FDs.
*/
void start_server(int portnumber, int numclient) {
  int rval, timeout, conn_idx;
  char buf[MAX_OUT_LINE];
  char* cmd;
  struct sockaddr_in sockIN, from;
  struct sigaction sa;
  socklen_t fromlen;

  nclient = numclient;
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
    pfd[i+2].fd = -1;
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

  // Handle SIGALM by printing the activity report
  sa.sa_handler = &print_report;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction(SIGALRM, &sa, NULL) == -1) {
    print_error(E_SIGACTION);
  }

  while (1) {

    // Send a SIGALRM every 15 seconds
    if (!alarmset) {
      alarm(15);
      alarmset = true;
    }

    // Check for keepalive messages, if found KAL_COUNT, close that connection
    check_kam(nclient);
    for (int i = 0; i < nclient; i++) {
      if (connections[i].kam_misses == KAL_COUNT) {
        add_crashlist(i);
        close_connection(i);
      }
    }

    rval= poll(pfd, numpolls, timeout);
    if (rval == -1) {
      // EINTR is set to errno when a read() or poll() is interrupted by a signal.
      // Need this since it will be interrupted by SIGALRM from alarm()
      if (errno == EINTR) continue;
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
          time(&connections[conn_idx].last_activity);
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
        }
      }
    }
  }
}

/* Parses the command sent by the client */
void parse_cmd(char* buf, int pfd_index) {
  bool activity = false;
  char* args;
  char* cmd = strtok(buf, "|\n");
  char ack[KAL_LENGTH + 1];
  /* int index = pfdtoconn_idx(pfd_index); */
  int index = pfd_index - 2;

  snprintf(ack, sizeof(ack), "%c%c%c%c%c", KAL_CHAR, KAL_CHAR, KAL_CHAR, KAL_CHAR, KAL_CHAR);

  if (strcmp(cmd, "open") == 0) {
    args = strtok(NULL, "| \n");
    if (args == NULL) {
      printf("username not found. Please try again");
      return;
    }
    activity = server_open(index, args);
  }
  else if (strcmp(cmd, "who") == 0) {
    activity = server_list_logged(index);
  }
  else if (strcmp(cmd, "to") == 0) {
    args = strtok(NULL, "\n");
    activity = server_add_receipient(index, args);
  }
  else if (strcmp(cmd, "<") == 0) {
    args = strtok(NULL, "\n");
    activity = server_receive_msg(index, args);
  }
  else if (strcmp(cmd, "close") == 0) {
    activity = server_close_client(index);
  }
  else if (strcmp(cmd, "exit") == 0) {
    activity = server_exit_client(index);
  }
  else if (strcmp(cmd, ack) == 0) {
    connections[index].kam_misses = 0;
  }

  if (activity) {
    time(&connections[index].last_activity);
  }
}

/* Server opens a chat session for a user. */
bool server_open(int index, char* username) {
  char msg_out[MAX_BUF];
  if (username_taken(username)) {
    snprintf(msg_out, sizeof(msg_out), "[server] Error: username is already taken.\n");
    if(write(connections[index].fd, msg_out, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE);
    }
    close_connection(index);
  }
  else {
    // Username is not taken
    strcpy(connections[index].username, username);

    /* // Write server msg to socket */
    write_connected_msg(username);
  }
  return true;
}

/* Server returns a list of logged users to the client */
bool server_list_logged(int index) {
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
  return true;
}

bool server_add_receipient(int index, char* receipients) {
  char* tok = strtok(receipients, " ");
  while (tok != NULL) {
    // Increment num of receipients and add to the list
    connections[index].num_receipients++;
    strcpy(connections[index].receipients[connections[index].num_receipients - 1], tok);
    tok = strtok(NULL, " \n");
  }
  return true;
}

bool server_receive_msg(int index, char* msg) {
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
          printf("a receipient may have crashed. cannot send message\n");
        }
      }
    }
  }
  return true;
}

/* Server closes the chat session that is initialized by the client */
bool server_close_client(int index) {
  char msg_out[MAX_BUF];

  memset(msg_out, 0, sizeof(msg_out));
  // Write server msg to socket
  snprintf(msg_out, sizeof(msg_out), "[server] done\n");

  if(write(connections[index].fd, msg_out, MAX_OUT_LINE) == -1) {
    print_error(E_WRITE);
  }

  close_connection(index);
  return true;
}

bool server_exit_client(int index) {
  close_connection(index);
  return true;
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
  pfd[index + 2].fd = -1;
  free_connection(index);
  pollfd_conn_defrag(pfd, NMAX);
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

// Function to defragment the pfd and connections arrays
void pollfd_conn_defrag(struct pollfd *pfd, int maxclient) {
  int new_size = 2;
  for (int x = 2; x < maxclient + 2; x++) {
    if (pfd[x].fd < 0) {
      continue;
    }
    pfd[new_size] = pfd[x];
    if (pfd[new_size].fd == pfd[new_size+1].fd) {
      pfd[x].fd = -1;
    }
    new_size++;
  }

  new_size = 0;
  for (int y = 0; y < maxclient; y++) {
    if(connections[y].fd < 0) {
      continue;
    }
    connections[new_size] = connections[y];
    if (connections[new_size].fd == connections[new_size+1].fd) {
      free_connection(y);
    }
    new_size++;
  }
}

// Function to check for keepalive messages for every connected client
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

// Empty a given element in the connections array
void free_connection(int index) {
  connections[index].fd = -1;
  connections[index].connected = false;
  memset(connections[index].username, 0, sizeof(connections[index].username));
  clear_receipients(index);
  connections[index].kam_misses = 0;
  connections[index].checked_kam = true;
}

// Signal handler for printing the activity report
void print_report(int signalnum) {
  struct tm *tmp;
  char time_buf[30];
  char out_buf[MAX_BUF];

  switch(signalnum) {
    case SIGALRM:
      printf("Activity Report:\n");
      // Print activity for connected clients
      for (int i = 0; i < nclient; i++) {
        if (connections[i].connected) {
          tmp = localtime(&connections[i].last_activity);
          if (strftime(time_buf, sizeof(time_buf), "%c", tmp) == 0) {
            print_error(E_TIMEBUF);
          }

          snprintf(out_buf, sizeof(out_buf), "'%s' [sockfd=%i]:%s", connections[i].username,
                    connections[i].fd, time_buf);
          printf("%s\n", out_buf);
        }
      }
      // Print the activity for users that crashed
      for (int j = 0; j < crashcount + 1; j++) {
        printf("%s\n", crashes[j]);
      }
      alarmset = false;
      break;
  }
}

// Add a client that has crashed to the crashed array, to be printed
void add_crashlist(int index) {
  time_t timecrash;
  struct tm *tmp;
  char time_buf[30];
  char out_buf[MAX_BUF];

  time(&timecrash);
  tmp = localtime(&timecrash);
  if (strftime(time_buf, sizeof(time_buf), "%c", tmp) == 0) {
    print_error(E_TIMEBUF);
  }

  snprintf(out_buf, sizeof(out_buf), "'%s' [sockfd=%i]: loss of keepalive messages detected at %s, connection closed"
      , connections[index].username, connections[index].fd, time_buf);

  strcpy(crashes[crashcount], out_buf);
  crashcount++;
}
