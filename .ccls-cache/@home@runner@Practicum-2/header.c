#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to build a Header_t object
Header_t *buildHeader(char *dir, int access) {
  Header_t *header = (Header_t *)malloc(sizeof(Header_t));
  if (header != NULL) {
    header->directory = strdup(dir);
    header->access = access;
    header->last_modified_time =
        time(NULL); // Set the last modified time to the current time
  }
  return header;
}

// Function to convert Header_t to a string
char *headerToString(Header_t *header) {
  if (header == NULL || header->directory == NULL) {
    return NULL;
  }
  // Allocate memory for the string
  size_t len = strlen(header->directory) +
               64; // Assuming a maximum of 64 characters for access and time
  char *str = (char *)malloc(len);
  if (str != NULL) {
    // Format the string with directory, access, and last modified time
    snprintf(str, len, "%s\n%d\n%s", header->directory, header->access,
             ctime(&header->last_modified_time));
  }
  return str;
}

// Function to free memory allocated for a Header_t object
void freeHeader(Header_t *header) {
  if (header != NULL) {
    free(header->directory);
    free(header);
  }
}
