#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "a1shell.h"

void done(pid_t pid) {
  int tmp = getpid();
  printf("getpid() result: %i\n", tmp);
  printf("pid result: %i\n", pid);
  int returned = kill(tmp, 9);
  printf("kill returned %i\n", returned);
}

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

void print_dir() {
  char buf[512];
  if (getcwd(buf, sizeof(buf)) != NULL) {
    printf("%s\n", buf);
  }
  else {
    print_error(E_PWD);
  }
}
