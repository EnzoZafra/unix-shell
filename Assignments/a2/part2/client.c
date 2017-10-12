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

#include "a2rchat.h"
#include "client.h"
#include "server.h"

void start_client(char* baseName) {
  char input[MAX_COMMAND_LINE];
  char* command;

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
      open_chat(username);
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
    close();
  }
  else if (strcmp(input, "exit") == 0) {
    exit_client();
  }
  else {
    printf("command not found: %s\n", input);
  }
}

void open_chat(char* username) {
  printf("username: %s\n", username);   // Testing
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

void close() {
  printf("Not yet implemented\n");
}

void exit_client() {
  printf("Not yet implemented\n");
}


