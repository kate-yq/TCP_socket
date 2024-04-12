// implementation of clientHandler.h
#include "utils.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "clientHandler.h"

#define LOCAL_DIR "local"
#define BUFFER_SIZE 4096
#define SERVER_PORT 8080

// send client request message to server
int send_request(int sockfd, const char *request)
{
  if (send(sockfd, request, strlen(request), 0) < 0)
  {
    perror("send");
    return -1;
  }
  return 0;
}

// send local file to server
int client_write(int sockfd, const char *local_path, const char *remote_path,
                 int permission)
{
  char request[BUFFER_SIZE];
  snprintf(request, BUFFER_SIZE, "WRITE %s %d", remote_path, permission);

  if (send_request(sockfd, request) < 0)
  {
    return -1;
  }

  // Wait for acknowledgment from server
  char ack[BUFFER_SIZE];
  if (recv(sockfd, ack, BUFFER_SIZE, 0) < 0)
  {
    perror("error receive server acknowledge");
    return -1;
  }

  // clean buffer
  memset(ack, '\0', sizeof(ack));

  // Construct the full file path by concatenating ROOT_DIR and file_path
  char *local_path_full = malloc(strlen(LOCAL_DIR) + strlen(local_path) + 1);
  sprintf(local_path_full, "%s/%s", LOCAL_DIR, local_path);

  int fd = open(local_path_full, O_RDONLY);
  if (fd < 0)
  {
    perror("open");
    free(local_path_full);
    return -1;
  }

  free(local_path_full);
  char buffer[BUFFER_SIZE];
  ssize_t read_bytes;
  if ((read_bytes = read(fd, buffer, BUFFER_SIZE)) > 0)
  {
    if (send(sockfd, buffer, read_bytes, 0) < 0)
    {
      perror("send");
      close(fd);
      return -1;
    }
  }
  else
  {
    perror("read");
    close(fd);
    return -1;
  }

  // clean buffer
  memset(buffer, '\0', sizeof(buffer));

  // Wait for final result message from server
  if (recv(sockfd, ack, BUFFER_SIZE, 0) < 0)
  {
    perror("error receive server results");
    return -1;
  }

  printf("Answer from server: %s\n", ack);

  // clean buffer
  memset(ack, '\0', sizeof(ack));

  close(fd);
  return 0;
}

// get remote file from server
int client_get(int sockfd, const char *file_path, const char *local_path)
{
  char request[BUFFER_SIZE];
  snprintf(request, BUFFER_SIZE, "GET %s", file_path);

  if (send_request(sockfd, request) < 0)
  {
    return -1;
  }

  char buffer[BUFFER_SIZE];

  // Construct the full file path by concatenating ROOT_DIR and file_path
  char *local_path_full = malloc(strlen(LOCAL_DIR) + strlen(local_path) + 1);
  sprintf(local_path_full, "%s/%s", LOCAL_DIR, local_path);

  if (createDir(local_path_full) != 0)
  {
    perror("open local dir");
    free(local_path_full);
    return -1;
  }

  int fd = open(local_path_full, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0)
  {
    perror("open");
    free(local_path_full);
    return -1;
  }

  free(local_path_full);
  ssize_t read_bytes;
  if ((read_bytes = recv(sockfd, buffer, BUFFER_SIZE, 0)) > 0)
  {
    if (write(fd, buffer, read_bytes) < 0)
    {
      perror("error in write");
      close(fd);
      return -1;
    }
  }
  else
  {
    perror("recv");
    close(fd);
    return -1;
  }

  close(fd);

  // send acknowledgement to server
  char ack[] = "successfully received";
  if (send(sockfd, ack, strlen(ack), 0) < 0)
  {
    perror("send");
    close(fd);
    return -1;
  }

  // clean buffer
  memset(buffer, '\0', sizeof(buffer));

  // Wait for final result message from server
  if (recv(sockfd, buffer, BUFFER_SIZE, 0) < 0)
  {
    perror("error receive server results");
    return -1;
  }

  printf("Answer from server: %s\n", buffer);

  return 0;
}

// remove remote file from server
int client_rm(int sockfd, const char *file_path)
{
  char request[BUFFER_SIZE];
  snprintf(request, BUFFER_SIZE, "RM %s", file_path);

  if (send_request(sockfd, request) < 0)
  {
    return -1;
  }

  char buffer[BUFFER_SIZE];

  // Wait for final result message from server
  if (recv(sockfd, buffer, BUFFER_SIZE, 0) < 0)
  {
    perror("error receive server results");
    return -1;
  }

  printf("Answer from server: %s\n", buffer);

  return 0;
}

// get permission meta data from server
int client_ls(int sockfd, const char *file_path)
{
  char request[BUFFER_SIZE];
  snprintf(request, BUFFER_SIZE, "LS %s", file_path);

  if (send_request(sockfd, request) < 0)
  {
    return -1;
  }

  char buffer[BUFFER_SIZE];
  if (recv(sockfd, buffer, BUFFER_SIZE, 0) < 0)
  {
    perror("receive error");
  }

  // print result
  printf("%s\n", buffer);

  return 0;
}

// stop server
int client_stop(int sockfd)
{
  char *request = "STOP";

  if (send_request(sockfd, request) < 0)
  {
    return -1;
  }

  return 0;
}