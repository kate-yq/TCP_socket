#ifndef HEADER_H
#define HEADER_H

#include <time.h>

// Define a structure for header of file
typedef struct {
  char *directory;
  int access;
  time_t last_modified_time;
} Header_t;

// Function to build a Header_t object
Header_t *buildHeader(char *dir, int access);

// Function to convert Header_t to a string
char *headerToString(Header_t *header);

// Function to free memory allocated for a Header_t object
void freeHeader(Header_t *header);

#endif /* HEADER_H */