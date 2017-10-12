#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
  int fd;
  char * myfifo = "fifo";

  /* mkfifo(myfifo, 0666); */
  /* fd = open(myfifo, O_WRONLY); */
  /* write(fd, "Hi", sizeof("Hi")); */
  /* close(fd); */

  unlink(myfifo);
  return 0;
}
