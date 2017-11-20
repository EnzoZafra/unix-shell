/**
  *
  * a3chat.h
  *
  * Lorenzo Zafra 1395521
  * CMPUT 379 Assignment 3
  */

#ifndef a3chat_h
#define a3chat_h

#include <stdio.h>
#include <stdlib.h>

#define NMAX 5
#define KAL_CHAR 0x6 // A non-printable character (e.g., ACK)
#define KAL_LENGTH 5 // Number of KAL_char in one keepalive message
#define KAL_INTERVAL 1.5 // Client sends a keepalive message every 1.5 seconds
#define KAL_COUNT 5 // Number of consecutive keepalive messages that needs
                    // to be missed for the server to consider that the client
                    // has terminated unexpectedly

typedef enum {
  E_USG_S = 1,
  E_USG_C = 2,
  E_USG = 3,
  E_NEGATIVE = 4,
  E_SOCKET = 5,
  E_WRITE = 6,
  E_POLL = 7,
  E_TIMEBUF = 8,
  E_READ = 9,
  E_FAIL_BIND = 10,
  E_INVALID_PORTNUM = 11,
  E_OPENSOCK = 12,
  E_GETHOST = 13,
  E_CONNECT = 14,
  E_SIGACTION = 15,
  E_IDX_NOTFOUND = 16
} error_t;

void print_error(int errorcode);
void write_fifo(int fd, char* msg);
void read_fifo(int fd, char* buf);
#endif
