/**
  *
  * a1shell.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment1
  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/stat.h>

#include "a1shell.h"
#include "a1commands.h"

#define BUFF_SIZE 32
#define MAX_CHAR 80

pid_t pid;

int main(int argc, char *argv[]) {
  char time_buffer[BUFF_SIZE];
  int interval;

  if (argc != 2) {
    print_error(E_USER);
  }

  interval = atoi(argv[1]);
  if (interval <= 0) {
    print_error(E_NEGATIVE);
  }
  else {

    // set a limit on CPU time (1 minute for now)
    struct rlimit rlim;
    getrlimit(RLIMIT_CPU, &rlim);
    rlim.rlim_cur = 60;
    if (setrlimit(RLIMIT_CPU, &rlim) == -1) {
      print_error(E_CPU_LIMIT);
    }

    pid = fork();
    if (pid < 0) {
      print_error(E_FORK_FAIL);
    }

    // child process - a1monitor
    else if (pid == 0) {
      while(getppid() != -1) {
        get_time(time_buffer);
        printf("a1monitor: %s\n", time_buffer);
        get_sysinfo();
        sleep(interval);
      }
      // _Exit() should be used for child processes
      _Exit(EXIT_SUCCESS);
    } // end of a1monitor process

    // parent process - a1shell
    else {
      sleep(1);
      while(1) {
        char input[MAX_CHAR];

        printf("a1shell%% ");
        scanf("%s", input);

        // Parse input for the command
        parse_input(input);
      }
      exit(EXIT_SUCCESS);
    } // end of a1shell process
  }
}

void get_time(char* buffer) {
  time_t rawtime;
  struct tm *outtime;

  time(&rawtime);
  outtime = localtime(&rawtime);

  strftime(buffer, BUFF_SIZE, "%a, %b %d, %Y %T %p", outtime);
}

void get_sysinfo() {
  FILE* filestream = fopen("/proc/loadavg", "r");
  float one_min, five_min, fifteen_min;
  char processes[10];
  int last_proc[10];

  if (filestream == NULL) {
    print_error(E_FILESTREAM);
  }
  else {
    fscanf(filestream, "%f %f %f %s %i",
        &one_min, &five_min, &fifteen_min, processes, last_proc);
    fclose(filestream);
    printf("           Load average: %0.2f, %0.2f, %0.2f\n", one_min, five_min, fifteen_min);
    printf("           Processes: %s\n", processes);

  }
}

void print_error(int errorcode) {
  switch (errorcode) {
    case 1:
      fprintf(stderr, "%s", "usage: a1shell interval\n");
      break;
    case 2:
      fprintf(stderr, "%s", "Failed to set CPU time limit\n");
      break;
    case 3:
      fprintf(stderr, "%s", "Failed to fork a child process\n");
      break;
    case 4:
      fprintf(stderr, "%s", "interval must be a positive integer\n");
      break;
    case 5:
      fprintf(stderr, "%s", "could not print current directory\n");
      break;
    case 6:
      fprintf(stderr, "%s", "could not open file\n");
      break;
    case 7:
      fprintf(stderr, "%s", "fail at waitpid during execl\n");
    case 8:
      fprintf(stderr, "%s", "fail to retrieve clktck\n");
  }
  exit(EXIT_FAILURE);
}

void parse_input(char* input) {
  if(strcmp(input, "cd") == 0) {
    char pathname[512];
    change_dir(pathname);
  }
  else if(strcmp(input, "done") == 0) {
    done(pid);
  }
  else if(strcmp(input, "pwd") == 0) {
    print_dir();
  }
  else if(strcmp(input, "umask") == 0) {
    print_umask();
  }
  else if (strcmp(input, "\n") == 0) {
    printf("TEST\n");
  }
  else {
    bash_command(input);
  }
}
