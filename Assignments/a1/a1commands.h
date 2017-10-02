/**
  *
  * a1commands.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment1
  */

#ifndef a1commands_h
#define a1commands_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

void done(pid_t pid);
int change_dir(char* pathname);
void print_umask();
void print_dir();
void bash_command(char* command);
void print_pr_times(clock_t real_time, struct tms *tmsstart, struct tms *tmsend);

#endif
