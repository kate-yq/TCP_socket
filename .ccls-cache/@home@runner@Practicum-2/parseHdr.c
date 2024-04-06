#include "parseHdr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// Function to convert permission to a string
// could add more info later
char *headerToString(int permission) {
  char *content = (char *)malloc(sizeof(char) * 2);
  sprintf(content, "%d", permission);
  return content;
}

// get the permission of the file
// 0 if read only
// 1 if read and write
// -1 if not exist
int getPermission(char *dir) {
  FILE *file = fopen(dir, "r");
  if (file == NULL) {
    return -1; // Error opening file
  }

  int permission;
  if (fscanf(file, "%d", &permission) != 1) {
    fclose(file);
    perror("Error reading number");
    return -1; // Error reading number
  }

  fclose(file);
  return permission;
}
