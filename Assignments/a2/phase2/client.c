/**
  *
  * client.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 2 Phase 2
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

#include "a2chat.h"
#include "client.h"
#include "server.h"

char* baseFifoName;
int fd = -1;
struct pollfd out_fds[NMAX];
int numfds = 2;

void start_client(char* baseName) {
  // One for outFIFO, one for STDIN
  char buf[MAX_BUF];
  char* command;
  int rval, timeout;
  bool prompt_user = true;

  baseFifoName = baseName;
  printf("Chat client begins\n");

  // For now, client does not need to multiplex. Just echoing from server
  // Add STDIN file descriptor to array
  out_fds[0].fd = STDIN_FILENO;
  out_fds[0].events = POLLIN;
  out_fds[0].revents = 0;

  timeout = 250;

  while(1) {
    // Only print after parsing input or receiving a msg.
    if (prompt_user) {
      // If not printing with a new line, we have to flush the buffer
      printf("a2chat_client: ");
      fflush(stdout);
      prompt_user = false;
    }

    rval = poll(out_fds, numfds, timeout);
    if (rval == -1) {
      print_error(E_POLL);
    }
    else if (rval != 0) {
      for (int j = 0; j < numfds; j++) {
        if(out_fds[j].revents & POLLIN) {
          // Clear buffer
          memset(buf, 0, sizeof(buf));
          prompt_user = true;
          if (read(out_fds[j].fd, buf, MAX_BUF) > 0) {
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

void parse_input(char* input) {
  char* args;
  if (strcmp(input, "open") == 0) {
    if (fd != -1) {
      printf("please close current chat session before starting a new one.\n");
      return;
    }
    args = strtok(NULL, "\n");
    if (args != NULL) {
      args[strcspn(args, "\n")] = 0;
      fd = open_chat(args);
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
      if (lockf(file_desc, F_LOCK, MAX_BUF) != -1) {
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
  if (fd != -1) {
    char outmsg[MAX_OUT_LINE];
    snprintf(outmsg, sizeof(outmsg), "who|\n");

    if(write(fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

void add_receipient(char* receipients) {
  if (fd != -1) {
    char outmsg[MAX_OUT_LINE];
    snprintf(outmsg, sizeof(outmsg), "to|%s\n", receipients);

    if(write(fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

void send_chat(char* message) {
  if (fd != -1) {
    char outmsg[MAX_OUT_LINE];
    snprintf(outmsg, sizeof(outmsg), "<|%s\n", message);

    if(write(fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

void close_client() {
  char outmsg[MAX_OUT_LINE];
  char buf[MAX_OUT_LINE];

  if (fd != -1) {
    snprintf(outmsg, sizeof(outmsg), "close|");
    if(write(fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }

    // Read once for server reply and close
    memset(buf, 0, sizeof(buf));
    int rval = poll(out_fds, numfds, 250);
    if (rval == -1) {
      print_error(E_POLL);
    }
    else if (rval != 0) {
      if(out_fds[1].revents & POLLIN) {
        // Clear buffer
        memset(buf, 0, sizeof(buf));
        if (read(out_fds[1].fd, buf, MAX_BUF) > 0) {
          // Print server reponse
          printf("%s", buf);

          // Close in and out fifo
          close_io_fifo();
        }
      }
    }
  }
  else {
    printf("You are not connected to a chat session.\n");
  }
}

void exit_client() {
  char outmsg[MAX_OUT_LINE];

  if (fd != -1) {
    snprintf(outmsg, sizeof(outmsg), "exit|");
    if(write(fd, outmsg, MAX_OUT_LINE) == -1) {
      print_error(E_WRITE_IN);
    }

    // Close in and out fifos
    close_io_fifo();
  }
  exit(EXIT_SUCCESS);
}

void close_io_fifo() {
  // Close out fifo
  close(out_fds[1].fd);
  out_fds[1].fd = -1;
  // Close and unlock infifo
  lockf(fd, F_ULOCK, MAX_BUF);
  close(fd);
  fd = -1;
}

void parse_server_msg(char* buf) {
  if (strncmp(buf, "[server] Error:", 15) == 0) {
    close_io_fifo();
  }
  printf("\n%s", buf);
}
