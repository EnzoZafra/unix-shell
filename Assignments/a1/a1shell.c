#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "a1shell.h"

#define BUFF_SIZE 32

int print_error(int errorcode) {
  switch (errorcode) {
    case 1:
      printf("usage: a1shell interval\n");
      break;
    case 2:
      printf("Failed to set CPU time limit\n");
      break;
    case 3:
      printf("Failed to fork a child process\n");
      break;
  }
  return -1;
}

int main(int argc, char *argv[]) {
  pid_t pid;
  struct tm *time;
  char time_buffer[BUFF_SIZE];

  if (argc != 2) {
    return print_error(E_USER);
  } else {
    int interval = atoi(argv[1]);

    // set a limit on CPU time (1 minute for now)
    struct rlimit rlim = {60, 60};
    if (setrlimit(RLIMIT_CPU, &rlim) == -1) {
      print_error(E_CPU_LIMIT);
    }

    pid = fork();
    // child process - a1monitor
    if (pid == 0) {
      while(getppid() != -1) {
        get_time(time_buffer);
        printf("a1monitor: %s\n", time_buffer);
      }
      exit(0);
    } // end of a1monitor process

    else if (pid == -1) {
      print_error(E_FORK_FAIL);
    }

    // parent process - a1shell
    else {

    } // end of a1shell process

    printf("REACHED END OF MAIN\n");
  }
}

void get_time(char* buffer) {
  time_t rawtime;
  struct tm *outtime;

  time(&rawtime);
  outtime = localtime(&rawtime);

  strftime(buffer, BUFF_SIZE, "%a, %b %d, %Y %T %p", outtime);
}
