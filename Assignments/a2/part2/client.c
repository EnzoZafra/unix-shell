/**
  *
  * client.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 1
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "a2rchat.h"
#include "client.h"
#include "server.h"

char* baseFifoName;
int fd;

void start_client(char* baseName) {
  char input[MAX_COMMAND_LINE];
  char* command;

  baseFifoName = baseName;
  printf("Chat client begins\n");

  while(1) {
    printf("a2chat_client: ");
    if (!(fgets(input, sizeof(input), stdin) == NULL || strcmp(&input[0], "\n") == 0)) {
      command = strtok(input, " \n");
      parse_input(command);
    }
  }
}

void parse_input(char* input) {
  if (strcmp(input, "open") == 0) {
    char* username;
    username = strtok(NULL, "\n");
    if (username != NULL) {
      username[strcspn(username, "\n")] = 0;
      fd = open_chat(username);
    }
    else {
      printf("please enter a username");
    }
  }
  else if (strcmp(input, "who") == 0) {
    list_logged();
  }
  else if (strcmp(input, "to") == 0) {
    add_receipient();
  }
  else if (strcmp(input, "<") == 0) {
    char* message;
    message = strtok(NULL, "\n");
    if (message!= NULL) {
      message[strcspn(message, "\n")] = 0;
      send_chat(message);
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

int open_chat(char* username) {
  char infifo_name[MAX_FIFO_NAME];
  int file_desc;

  for (int i = 1; i <= NMAX; i++) {
    memset(infifo_name, 0, sizeof infifo_name);
    snprintf(infifo_name, sizeof infifo_name, "%s-%d.in", baseFifoName, i);
    file_desc = open(infifo_name, O_RDWR|O_NONBLOCK);

    if (lockf(file_desc, F_TEST, 0) == -1) {
      printf("fail to lock \n"); // Testing
      close(file_desc);
    }
    else {
      if (lockf(file_desc, F_LOCK, 0) != -1) {

        // Successfully locked and connected to a FIFO
        printf("FIFO [%s] has been successfully locked by PID [%d]\n", infifo_name, getpid());
        printf("file_desc: %d\n", file_desc); // Testing
        return file_desc;
      }
    }
  }

  // Was not able to find an unlocked FIFO
  printf("No unlocked inFIFO is available for use. Please try again later.\n");
  return -1;
}

void list_logged() {
  printf("This command is not part of phase 1.\n");
}

void add_receipient() {
  printf("This command is not part of phase 1.\n");
}

void send_chat(char* message) {
  printf("message: %s\n", message);   // Testing
}

void close_client() {
  printf("Not yet implemented\n");
}

void exit_client() {
  printf("Not yet implemented\n");
}


