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
int fd = -1;

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
      printf("usage: open [username]");
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
  char infifo[MAX_FIFO_NAME];
  char outmsg[MAX_OUT_LINE];
  int file_desc;

  for (int i = 1; i <= NMAX; i++) {
    memset(infifo, 0, sizeof infifo);
    snprintf(infifo, sizeof infifo, "%s-%d.in", baseFifoName, i);
    file_desc = open(infifo, O_WRONLY | O_NONBLOCK);

    if (lockf(file_desc, F_TEST, 0) == -1) {
      close(file_desc);
    }
    else {
      if (lockf(file_desc, F_LOCK, 0) != -1) {
        // Successfully locked and connected to a FIFO
        printf("FIFO [%s] has been successfully locked by PID [%d]\n", infifo, getpid());

        // Write command, fifo number, username to the fifo so server knows we connected
        snprintf(outmsg, sizeof(outmsg), "%s,%d,%s,", "open", i, username);

        if(write(file_desc, outmsg, MAX_OUT_LINE) == -1) {
          print_error(E_WRITE_IN);
        }
        else {
          printf("wrote %s to file_desc: %i\n", outmsg, file_desc); // Testing
        }

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
  char outmsg[MAX_OUT_LINE];
  if (fd != -1) {
    close(fd);
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
  // TODO: write more info?
  // write command to the pipe
  snprintf(outmsg, sizeof(outmsg), "close");
  if(write(fd, outmsg, MAX_OUT_LINE) == -1) {
    print_error(E_WRITE_IN);
  }
  else {
    printf("wrote %s to file_desc: %i\n", outmsg, fd); // Testing
  }
}

void exit_client() {
  close(fd);
  exit(EXIT_SUCCESS);
}
