/**
  *
  * a1shell.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment1
  */

#ifndef a1shell_h
#define a1shell_h

#include <stdio.h>
#include <stdlib.h>

typedef enum {
  E_USER = 1,
  E_CPU_LIMIT = 2,
  E_FORK_FAIL = 3,
  E_NEGATIVE = 4,
  E_PWD = 5,
  E_FILESTREAM = 6,
  E_WAIT_FAIL = 7,
  E_CLKTCK = 8
} error_t;

void get_time(char* buffer);
void get_sysinfo();
void print_error(int error);
void parse_input(char* input);
#endif
