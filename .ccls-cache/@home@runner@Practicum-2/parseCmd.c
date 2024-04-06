#include "parseCmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to convert a char** to a string
// string format: "number_args arg1 arg2 ..."
char *commandToString(char **args, int size) {
  // Calculate the length of the resulting string
  int totalLength = 1;
  for (int i = 0; i < size; i++) {
    totalLength++; // Account for space
    totalLength += strlen(args[i]);
  }

  // Allocate memory for the string
  char *str = (char *)malloc(totalLength + 1); // +1 for null terminator
  if (str == NULL) {
    // Memory allocation failed
    return NULL;
  }

  // Concatenate each argument with a space separator
  sprintf(str, "%d", size - 1);
  for (int i = 2; i < size; i++) {
    strcat(str, " ");
    strcat(str, args[i]);
  }

  strcat(str, "\0");

  return str;
}

// Function to convert a string back to a char**
// cahr** format:
//   number_args
//   arg1
//   arg2 ...
char **stringToCommand(char *str) {
  // Count the number of tokens (words) in the string
  int numTokens = atoi(str);

  // Allocate memory for the char** array
  char **args = (char **)malloc((numTokens + 1) * sizeof(char *));
  if (args == NULL) {
    // Memory allocation failed
    return NULL;
  }

  // Split the string into tokens and store them in the array
  int i = 0;
  char *token = strtok(str, " "); // size
  while (token != NULL) {
    args[i] = strdup(token);
    if (args[i] == NULL) {
      // Memory allocation failed
      // Free previously allocated memory and return NULL
      for (int j = 0; j < i; j++) {
        free(args[j]);
      }
      free(args);
      return NULL;
    }
    i++;
    token = strtok(NULL, " ");
  }
  args[i] = NULL; // Null-terminate the array

  return args;
}

// Function to free memory allocated for a char** array
void freeCharPtr(char **args) {
  for (int i = 0; args[i] != NULL; i++) {
    free(args[i]);
  }
  free(args);
}