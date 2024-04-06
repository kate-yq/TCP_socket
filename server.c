/*
 * server.c -- TCP Socket Server
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "serverHandler.h"

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
  int client_sockfd;
  char *command;
  char *file_path;
  char *permission_str;
} ThreadArgs_t;

void *handle_client(void *arg) {
  int client_sockfd = ((ThreadArgs_t *)arg)->client_sockfd;
  char *command = ((ThreadArgs_t *)arg)->command;
  char *file_path = ((ThreadArgs_t *)arg)->file_path;
  char *permission_str = ((ThreadArgs_t *)arg)->permission_str;

  int status = -1;
  if (strcmp(command, "WRITE") == 0) {
    int permission = -1;
    if (permission_str != NULL) {
      permission = atoi(permission_str);
    }
    status = handle_write(client_sockfd, file_path, permission);
  } else if (strcmp(command, "GET") == 0) {
    status = handle_get(client_sockfd, file_path);
  } else if (strcmp(command, "RM") == 0) {
    status = handle_rm(client_sockfd, file_path);
  } else if (strcmp(command, "LS") == 0) {
    status = handle_ls(client_sockfd, file_path);
  } else {
    printf("Invalid command: %s\n", command);
  }

  if (status < 0) {
    printf("Error occurred while handling command: %s\n", command);
  }

  free(arg);
  close(client_sockfd);
  pthread_exit(NULL);
}

// main function add in threads

int main() {
  int server_sockfd, client_sockfd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len;
  pthread_t thread_id;
  char buffer[BUFFER_SIZE];

  // Create socket
  server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  printf("Socket created successfully\n");

  // Configure server address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind the socket to the specified IP and port
  if (bind(server_sockfd, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // Listen for incoming connections
  if (listen(server_sockfd, 5) < 0) {
    perror("error in server listening");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d\n", PORT);

  while (1) {
    addr_len = sizeof(client_addr);
    client_sockfd =
        accept(server_sockfd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_sockfd < 0) {
      perror("error when accept client");
      continue;
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    // check command before create thread
    memset(buffer, '\0', sizeof(buffer));

    if (recv(client_sockfd, buffer, sizeof(buffer), 0) < 0) {
      perror("error receive command");
    }
    // printf("Received from client: %s\n", buffer);

    ThreadArgs_t *thread_args = (ThreadArgs_t *)malloc(sizeof(ThreadArgs_t));
    thread_args->command = strtok(buffer, " ");

    if (strcmp(thread_args->command, "STOP") == 0) {
      free(thread_args);
      close(client_sockfd);
      break;
    }

    if (strcmp(thread_args->command, "SKIP") == 0) {
      free(thread_args);
      close(client_sockfd);
      continue;
    }

    thread_args->client_sockfd = client_sockfd;
    thread_args->file_path = strtok(NULL, " ");
    thread_args->permission_str = strtok(NULL, " ");

    // Create a new thread to handle the client
    if (pthread_create(&thread_id, NULL, handle_client, (void *)thread_args) <
        0) {
      perror("error with pthread_create");
      close(client_sockfd);
      free(thread_args);
      continue;
    }

    // Detach the thread to prevent memory leaks
    if (pthread_detach(thread_id) != 0) {
      perror("Error detaching client thread");
    }
  }

  close(server_sockfd);
  return 0;
}