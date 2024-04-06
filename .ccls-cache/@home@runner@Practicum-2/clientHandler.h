#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

int send_request(int sockfd, const char *request);

int client_write(int sockfd, const char *local_path, const char *remote_path,
                 int permission);

int client_get(int sockfd, const char *file_path, const char *local_path);

int client_rm(int sockfd, const char *file_path);

int client_ls(int sockfd, const char *file_path);

int client_stop(int sockfd);

#endif