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
#include <poll.h>

#include "a2rchat.h"
#include "client.h"
#include "server.h"

char* baseFifoName;
int fd = -1;
struct pollfd out_fds[NMAX];

void start_client(char* baseName) {
  // One for STDIN and one for a outFIFO
  int numfds = 2;
  char buf[MAX_BUF];
  char* command;
  int rval, timeout;

  baseFifoName = baseName;
  printf("Chat client begins\n");

  // For now, client does not need to multiplex. Just echoing from server
  // Add STDIN file descriptor to array
  /* out_fds[0].fd = STDIN_FILENO; */
  /* out_fds[0].events = POLLIN; */
  /* out_fds[0].revents = 0; */

  timeout = 0;

  while(1) {
    printf("a2chat_client: ");

    if (!(fgets(buf, sizeof(buf), stdin) == NULL || strcmp(&buf[0], "\n") == 0)) {
      command = strtok(buf, " \n");
      parse_input(command);

      // Get server response -- BLOCKING? issues with poll as a client..
      rval = poll(out_fds, numfds, timeout);
      if (rval == -1) {
        print_error(E_POLL);
      }
      else if (rval != 0) {
        for (int j = 0; j < numfds; j++) {
          if(out_fds[j].revents & POLLIN) {
            // Clear buffer
            memset(buf, 0, sizeof(buf));
            if (read(out_fds[j].fd, buf, MAX_BUF) > 0) {
              // Print server reponse
              printf("%s\n", buf);
            }
          }
        }
      }
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
      printf("usage: open [username]\n");
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
  char infifo[MAX_NAME];
  char outfifo[MAX_NAME+1];
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

        // Listen to outFIFO
        snprintf(outfifo, sizeof outfifo, "%s-%d.out", baseFifoName, i);
        int outfd = open(outfifo, O_RDONLY | O_NONBLOCK);
        out_fds[1].fd = outfd;
        out_fds[1].events = POLLIN;
        out_fds[1].revents = 0;

        // Write command, username to the fifo so server knows we connected
        snprintf(outmsg, sizeof(outmsg), "%s|%s|", "open", username);

        if(write(file_desc, outmsg, MAX_OUT_LINE) == -1) {
          print_error(E_WRITE_IN);
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
  char buf[MAX_OUT_LINE];
  char *response;

  snprintf(outmsg, sizeof(outmsg), "close|");
  if(write(out_fds[1].fd, outmsg, MAX_OUT_LINE) == -1) {
    print_error(E_WRITE_IN);
  }

  // Read once for server reply and close
  if (fd != -1) {
    if (read(fd, buf, MAX_OUT_LINE) > 0) {
      response = strtok(buf, "\n");
      printf("%s\n", response);
      // Close out fifo
      close(out_fds[1].fd);
      out_fds[1].fd = -1;
      // Close and unlock infifo
      lockf(fd, F_ULOCK, 0);
      close(fd);
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

void exit_client() {
  char outmsg[MAX_OUT_LINE];

  snprintf(outmsg, sizeof(outmsg), "exit|");
  if(write(fd, outmsg, MAX_OUT_LINE) == -1) {
    print_error(E_WRITE_IN);
  }

  close(fd);
  exit(EXIT_SUCCESS);
}
