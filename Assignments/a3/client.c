/**
  *
  * client.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 3
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <netdb.h>

#include "a3chat.h"
#include "client.h"
#include "server.h"

struct pollfd sockOUT[NMAX];
int numfds = 2;

/* Main function for the client, contains the loop which polls the fifos
   and prompts the user for input
 */
void start_client(int portNum, char* serverAddress) {
  // One for outFIFO, one for STDIN
  char buf[MAX_BUF];
  char* command;
  int rval, timeout;
  bool prompt_user = true;
  struct hostent *hp;
  struct sockaddr_in server;
  FILE* sfp;

  hp = gethostbyname(serverAddress);
  if (hp == (struct hostent *) NULL) {
    print_error(E_GETHOST);
	  exit (1);
  }
  printf("Chat client begins (server '%s' [%s], port %i)\n", serverAddress, hp->h_addr, portNum);

  memset ((char *) &server, 0, sizeof server);
  memcpy ((char *) &server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_family= hp->h_addrtype;
  server.sin_port= htons(portNum);

  // Create socket and initialize connection

  int fd = socket(hp->h_addrtype, SOCK_STREAM, 0);
  if (fd < 0) {
    print_error(E_SOCKET);
    exit(1);
  }

  if (connect(fd, (struct sockaddr *) &server, sizeof server) < 0) {
    print_error(E_CONNECT);
    exit(1);
  }

  sockOUT[1].fd = fd;
  sockOUT[1].events = POLLIN;
  sockOUT[1].revents = 0;

  /* we may also want to perform STREAM I/O on the socket */
  // TODO: delete if dont want to use stream IO

  if ((sfp= fdopen(fd, "r")) < 0) {
    print_error(E_OPENSOCK);
    exit(1);
  }

  // Add stdin to poll struct
  sockOUT[0].fd = STDIN_FILENO;
  sockOUT[0].events = POLLIN;
  sockOUT[0].revents = 0;

  timeout = 250;

  while(1) {
    // Only print after parsing input or receiving a msg.
    if (prompt_user) {
      // If not printing with a new line, we have to flush the buffer
      printf("a3chat_client: ");
      fflush(stdout);
      prompt_user = false;
    }

    rval = poll(sockOUT, numfds, timeout);
    if (rval == -1) {
      print_error(E_POLL);
    }
    else if (rval != 0) {
      for (int j = 0; j < numfds; j++) {
        if(sockOUT[j].revents & POLLIN) {
          // Clear buffer
          memset(buf, 0, sizeof(buf));
          prompt_user = true;
          if (read(sockOUT[j].fd, buf, MAX_BUF) > 0) {
            // stdin
            if (j == 0) {
              command = strtok(buf, " \n");
              if (command != NULL) {
                parse_input(command);
              }
            }
            else {
              // parse the server's message
              parse_server_msg(buf);
            }
          }
        }
      }
    }
  }
}

/* Parses the user input and runs a command depending on the input */
void parse_input(char* input) {
  char* args;
  if (strcmp(input, "open") == 0) {
    if (sockOUT[1].fd != -1) {
      printf("please close current chat session before starting a new one.\n");
      return;
    }
    args = strtok(NULL, "\n");
    if (args != NULL) {
      args[strcspn(args, "\n")] = 0;
      open_chat(args);
    }
    else {
      printf("usage: open [username]\n");
    }
  }
  else if (strcmp(input, "who") == 0) {
    list_logged();
  }
  else if (strcmp(input, "to") == 0) {
    args = strtok(NULL, "\n");
    if (args != NULL) {
      args[strcspn(args, "\n")] = 0;
      add_receipient(args);
    }
  }
  else if (strcmp(input, "<") == 0) {
    args = strtok(NULL, "\n");
    if (args != NULL) {
      args[strcspn(args, "\n")] = 0;
      send_chat(args);
    }
  }
  else if (strcmp(input, "close") == 0) {
    close_client();
  }
  else if (strcmp(input, "exit") == 0) {
    exit_client();
  }
  else {
    printf("command not found: %s\n", input);
  }
}

void open_chat(char* username) {
  char outmsg[MAX_OUT_LINE];


  // Write command, username to the fifo so server knows we want to connect
  snprintf(outmsg, sizeof(outmsg), "%s|%s|", "open", username);

  if(write(sockOUT[1].fd, outmsg, MAX_OUT_LINE) == -1) {
    print_error(E_WRITE_IN);
  }
}

/* Tells the server that the client wants a list of logged users */
void list_logged() {
  if (sockOUT[1].fd != -1) {
    char outmsg[MAX_OUT_LINE];
    snprintf(outmsg, sizeof(outmsg), "who|\n");

    if(write(sockOUT[1].fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

/* Tells the server that the client wants to add a recipient to its list */
void add_receipient(char* receipients) {
  if (sockOUT[1].fd != -1) {
    char outmsg[MAX_OUT_LINE];
    snprintf(outmsg, sizeof(outmsg), "to|%s\n", receipients);

    if(write(sockOUT[1].fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

/* Sends a chat message to all of the clients recipients */
void send_chat(char* message) {
  if (sockOUT[1].fd != -1) {
    char outmsg[MAX_OUT_LINE];
    snprintf(outmsg, sizeof(outmsg), "<|%s\n", message);

    if(write(sockOUT[1].fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

/* Closes a chat session for the user */
void close_client() {
  char outmsg[MAX_OUT_LINE];
  char buf[MAX_OUT_LINE];

  if (sockOUT[1].fd != -1) {
    snprintf(outmsg, sizeof(outmsg), "close|");
    if(write(sockOUT[1].fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }

    // Read once for server reply and close
    memset(buf, 0, sizeof(buf));
    int rval = poll(sockOUT, numfds, 250);
    if (rval == -1) {
      print_error(E_POLL);
    }
    else if (rval != 0) {
      if(sockOUT[1].revents & POLLIN) {
        // Clear buffer
        memset(buf, 0, sizeof(buf));
        if (read(sockOUT[1].fd, buf, MAX_BUF) > 0) {
          // Print server reponse
          printf("%s", buf);

          // Close in and out fifo
          close_sockfd();
        }
      }
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

/* Exits the client program */
void exit_client() {
  char outmsg[MAX_OUT_LINE];

  if (sockOUT[1].fd != -1) {
    snprintf(outmsg, sizeof(outmsg), "exit|");
    if(write(sockOUT[1].fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }

    // Close in and out fifos
    close_sockfd();
  }
  exit(EXIT_SUCCESS);
}

/* Helper function to close socket fd */
void close_sockfd() {
  // Close socket fd
  close(sockOUT[1].fd);
  sockOUT[1].fd = -1;
}

/* Helper function for parsing the server message */
void parse_server_msg(char* buf) {
  if (strncmp(buf, "[server] Error:", 15) == 0) {
    close_sockfd();
  }
  printf("\n%s", buf);
}
