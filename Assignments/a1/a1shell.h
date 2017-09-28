#ifndef a1shell_h
#define a1shell_h

#include <stdio.h>
#include <stdlib.h>

typedef enum {
  E_USER = 1,
  E_CPU_LIMIT = 2,
  E_FORK_FAIL = 3
} error_t;

void get_time(char* buffer);
#endif
