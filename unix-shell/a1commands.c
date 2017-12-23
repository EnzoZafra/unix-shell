/**
  *
  * a1commands.c
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment1
  */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/wait.h>
#include "a1shell.h"

// Kills a1shell and therefore will also kill a1monitor
void done(pid_t pid) {
  kill(0, 9);
}

// Changes the current directory
int change_dir(char* pathname) {
  scanf("%s", pathname);
  char final_dir[512];

  // check for environment variable
  if (pathname[0] == '$') {
    // Remove the '$'
    memmove(pathname, pathname+1, strlen(pathname));

    // Grab everything up to the first /
    char *env_var = strtok(pathname, "/\n\t");
    // Get the directory of the environment variable
    char *env_var_dir = getenv(env_var);
    if (env_var_dir == NULL) {
      printf("cd: no such environment variable: %s\n", env_var);
      return -1;
    }

    // Grab the rest
    char *directory = strtok(NULL, "");
    char tmp[2] = "/";
    strcpy(final_dir, env_var_dir);

    if (directory != '\0') {
      // Concat the environment dir to the rest
      strcat(final_dir, tmp);
      strcat(final_dir, directory);
    }
  }
  else {
    strcpy(final_dir, pathname);
  }

  if (chdir(final_dir) != 0) {
    printf("cd: no such file or directory: %s\n", final_dir);
    return -1;
  }

  return 1;
}

// Prints the current directory to the user
void print_dir() {
  char buf[512];
  if (getcwd(buf, sizeof(buf)) != NULL) {
    printf("%s\n", buf);
  }
  else {
    print_error(E_PWD);
  }
}

// Prints the current filemode creation mask in octal to the user
void print_umask() {
  mode_t mask = umask(0);
  printf("umask: %04o\n", mask);
  printf("S_IRWXU: %04o\n", S_IRWXU);
  printf("S_IRWXG: %04o\n", S_IRWXG);
  printf("S_IRWXO: %04o\n", S_IRWXO);
}

// Prints the real time, user CPU time and system CPU time required to execute the command given
void print_pr_times(clock_t real, struct tms *tmsstart, struct tms *tmsend) {
  static long clktck = 0;
  if (clktck == 0) {
    if ((clktck = sysconf(_SC_CLK_TCK)) < 0) {
      print_error(E_CLKTCK);
    }
  }

  printf(" real:  %7.2f\n", real / (double) clktck);
  printf(" (a1shell) user: %7.2f\n",
      (tmsend->tms_utime - tmsstart->tms_utime) / (double) clktck);
  printf(" (a1shell) system: %7.2f\n",
      (tmsend->tms_stime - tmsstart->tms_stime) / (double) clktck);
  printf(" (child) user: %7.2f\n",
      (tmsend->tms_cutime - tmsstart->tms_cutime) / (double) clktck);
  printf(" (child) system: %7.2f\n",
      (tmsend->tms_cstime - tmsstart->tms_cstime) / (double) clktck);
}

// Executes the bash command given by the user.
void bash_command(char* command) {
  pid_t pid;
  clock_t start_time, end_time;
  struct tms tmsstart, tmsend;
  char args[512];

  // get rest of args
  fgets(args, sizeof(args), stdin);
  strcat(command, args);

  // get start time
  start_time = times(&tmsstart);

  // fork a child process
  pid = fork();
  if (pid < 0) {
    print_error(E_FORK_FAIL);
  }
  // bash process
  else if (pid == 0) {
    // execl will only return if there is a failure
    execl("/bin/bash", "bash", "-c", command, (char*) 0);
    exit(EXIT_FAILURE);
  }
  // a1shell process
  else {
    pid = waitpid(pid, NULL, WNOHANG);
    if (pid == -1) {
      print_error(E_WAIT_FAIL);
    }
    end_time = times(&tmsend);

    clock_t total_time = end_time - start_time;
    print_pr_times(total_time, &tmsstart, &tmsend);
  }
}
