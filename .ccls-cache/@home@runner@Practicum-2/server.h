#ifndef SERVER_H
#define SERVER_H

// File structure with a file descriptor for locking
typedef struct {
  char filename[256];
  int fd; // File descriptor for locking
} File;

// Function prototypes
void *handle_client(void *arg);

#endif 