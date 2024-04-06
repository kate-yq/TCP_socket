/** all command operations
 * write
 * get
 * rm
 * ls
 */
#ifndef SERVERCMD_H
#define SERVERCMD_H

int write(int client_sockfd, char *file_path);

int get(int client_sockfd, char *file_path);

int rm(char *file_path);

int ls(int client_sockfd, char *file_path);


#endif 