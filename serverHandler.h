/** all command operations
 * write
 * get
 * rm
 * ls
 */
#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

// think: permission int or char?
// mark: need 
int handle_write(int client_sockfd, char *file_path, int permission);

int handle_get(int client_sockfd, char *file_path);

int handle_rm(int client_sockfd, char *file_path);

int handle_ls(int client_sockfd, char *file_path);


#endif // SERVERHANDLER_H