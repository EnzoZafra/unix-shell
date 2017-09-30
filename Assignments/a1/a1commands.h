#ifndef a1commands_h
#define a1commands_h

#include <stdio.h>
#include <stdlib.h>

void done(pid_t pid);
int change_dir(char* pathname);
void print_umask();
void print_dir();
void bash_command(char* command);

#endif
