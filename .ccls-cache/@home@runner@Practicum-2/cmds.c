#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>

#include "cmds.h"
#include "server.h"

/** return status code
  * 0 for success, -1 for failure
  */
int write(){
  // create dir & file on server
  // lock the file - write lock

  // create dir & meta on server
  // lock the file - write lock

  while(1){
    // read buffer on local

    // write buffer to remote
  }

  // release lock
  // release lock
}

/** return status code
* 0 for success, -1 for failure
*/
int get(int client_sockfd, char *file_path){
  File *file = find_file(file_path);
  if (file == NULL) {
    perror("open");
    return -1;
  }

  // Lock the file for reading (shared lock)
  if (flock(file->fd, LOCK_SH) < 0) {
      perror("flock");
      return -1;
  }

  char buffer[BUFFER_SIZE];
  ssize_t read_bytes;
  while ((read_bytes = read(file->fd, buffer, BUFFER_SIZE)) > 0) {
      if (send(client_sockfd, buffer, read_bytes, 0) < 0) {
          perror("send");
          flock(file->fd, LOCK_UN);
          return -1;
      }
  }

  if (read_bytes < 0) {
      perror("read");
      flock(file->fd, LOCK_UN);
      return -1;
  }

  flock(file->fd, LOCK_UN); // Unlock the file
  return 0;
}

/** return status code
* 0 for success, -1 for failure
*/
int rm(){
  // delete dir & file on server
  // delete dir & meta on server
  
}

/** return status code
* 0 for success, -1 for failure
*/
int ls(){
  // list info 
  
}