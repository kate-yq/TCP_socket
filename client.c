/*
 * client.c -- TCP Socket Client
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "clientHandler.h"

#define BUFFER_SIZE 4096
#define SERVER_PORT 8080

// close function to send the fail message and close client socket
void client_close(int sockfd) {
  char fail_message[] = "SKIP ";
  if (send(sockfd, fail_message, strlen(fail_message), 0) < 0) {
    perror("send");
  }
  close(sockfd);
}

// Main function of client side
int main(int argc, char *argv[]) {
  // Check if the number of arguments is correct
  if (argc < 3 || strcmp(argv[1], "rfs") != 0) {
    printf("Usage: %s rfs <command> [local_path] [remote_path]\n", argv[0]);
    return 1;
  }

  // Get the command from the argument
  char *command = argv[2];

  // Create socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    return 1;
  }

  // Set up server address
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);

  // Connect to server
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("connect");
    client_close(sockfd);
    return 1;
  }
  printf("Connected with server successfully\n");

  // Implement command locally
  int status = 0;
  if (strcmp(command, "WRITE") == 0) {
    // Command: WRITE
    if (argc < 5 || argc > 6) {
      printf("Usage: %s rfs WRITE <local_path> <remote_path> <permission>\n",
             argv[0]);
      client_close(sockfd);
      return 1;
    }
    // Processing WRITE command...
    char *local_path = argv[3];
    // remote path optional
    char *remote_path = argv[argc - 2];
    char *endptr;
    long val = strtol(argv[argc - 1], &endptr, 10);

    // Check for conversion errors
    if (endptr == argv[argc - 1] || *endptr != '\0') {
      printf("Invalid permission: %s\n", argv[argc - 1]);
      client_close(sockfd);
      return 1;
    }

    // Check for out-of-range values
    if ((val == LONG_MIN || val == LONG_MAX) && errno == ERANGE) {
      printf("Permission value out of range\n");
      client_close(sockfd);
      return 1;
    }

    // Convert the value to int
    int permission = (int)val;

    if (permission < 0 || permission > 1) {
      printf("Invalid permission: %d\nOnly 0 or 1 allowed", permission);
      client_close(sockfd);
      return 1;
    }

    status = client_write(sockfd, local_path, remote_path, permission);

  } else if (strcmp(command, "GET") == 0) {
    // Command: GET
    if (argc < 4 || argc > 5) {
      printf("Usage: %s rfs GET <remote_path> <local_path>\n", argv[0]);
      client_close(sockfd);
      return 1;
    }
    // Processing GET command...
    char *remote_path = argv[3];
    // local path optional
    char *local_path = argv[argc - 1];
    status = client_get(sockfd, remote_path, local_path);

  } else if (strcmp(command, "RM") == 0) {
    // Command: RM
    if (argc != 4) {
      printf("Usage: %s rfs RM <file_path>\n", argv[0]);
      client_close(sockfd);
      return 1;
    }
    // Processing RM command...
    char *file_path = argv[3];
    status = client_rm(sockfd, file_path);

  } else if (strcmp(command, "LS") == 0) {
    // Command: LS
    if (argc != 4) {
      printf("Usage: %s rfs LS <file_path>\n", argv[0]);
      client_close(sockfd);
      return 1;
    }
    // Processing LS command...
    char *file_path = argv[3];
    status = client_ls(sockfd, file_path);

  } else if (strcmp(command, "STOP") == 0) {
    // Command: STOP
    if (argc != 3) {
      printf("Usage: %s rfs STOP\n", argv[0]);
      client_close(sockfd);
      return 1;
    }
    // Processing STOP command...
    status = client_stop(sockfd);

  } else if (strcmp(command, "HELP") == 0) {
    // Command: HELP
    if (argc != 3) {
      printf("Usage: %s rfs HELP\n", argv[0]);
      client_close(sockfd);
      return 1;
    }
    printf("Available commands:\n");
    printf("rfs WRITE <local_path> <remote_path> <permission>\n");
    printf("rfs GET <remote_path> <local_path>\n");
    printf("rfs RM <remote_path>\n");
    printf("rfs LS <remote_path>\n");
    printf("rfs STOP\n");
    client_close(sockfd);
    return 0;
  } else {
    printf("Invalid command: %s\n", command);
    client_close(sockfd);
    return 1;
  }

  // Close socket before returning
  close(sockfd);
  return status;
}