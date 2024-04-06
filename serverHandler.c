#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cipher.h"
#include "parseHdr.h"
#include "serverHandler.h"

#define BUFFER_SIZE 4096
#define ROOT_DIR "file"
#define META_DIR "meta"
// "root" "disk"

/** return status code
 * 0 for success, -1 for failure
 */
int handle_write(int client_sockfd, char *file_path, int permission) {
  char client_message[BUFFER_SIZE];
  char server_message[BUFFER_SIZE];

  memset(client_message, '\0', sizeof(client_message));
  memset(server_message, '\0', sizeof(client_message));

  sprintf(server_message, "Message received\n");
  if (send(client_sockfd, server_message, strlen(server_message), 0) < 0) {
    perror("error sending acknowledge");
    return -1;
  }

  // Construct the full meta file path
  char *meta_path_full = malloc(strlen(META_DIR) + strlen(file_path) + 1);
  sprintf(meta_path_full, "%s/%s", META_DIR, file_path);

  // Check permission
  if (getPermission(meta_path_full) == 0) {
    // The file already exists and the permission is READ ONLY
    free(meta_path_full);

    // receive the file for nothing, just to send the final message
    if (recv(client_sockfd, client_message, BUFFER_SIZE, 0) < 0) {
      perror("Couldn't receive");
      return -1;
    }

    // Send failure message to client
    sprintf(server_message, "Access denied: %s\nThis is a READ ONLY file\n",
            file_path);
    if (send(client_sockfd, server_message, strlen(server_message), 0) < 0) {
      printf("Can't send\n");
      return -1;
    }

    return 0;
  }

  // Parse the meta file path into directory and filename
  char *dir_path_meta = strdup(meta_path_full);
  char *metaname = strrchr(dir_path_meta, '/');
  if (metaname) {
    *metaname++ = '\0';
  } else {
    metaname = dir_path_meta;
    dir_path_meta = ".";
  }

  // Create the meta directory if it doesn't exist
  if (mkdir(dir_path_meta, 0755) != 0 && errno != EEXIST) {
    perror("mkdir meta");
    free(meta_path_full);
    free(dir_path_meta);
    return -1;
  }

  // Open and lock the meta file
  int mfd = open(meta_path_full, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (mfd == -1) {
    perror("open meta");
    free(meta_path_full);
    free(dir_path_meta);
    return -1;
  }

  struct flock mlock;
  mlock.l_type = F_WRLCK;
  mlock.l_whence = SEEK_SET;
  mlock.l_start = 0;
  mlock.l_len = 0;

  if (fcntl(mfd, F_SETLKW, &mlock) == -1) {
    perror("fcntl meta");
    close(mfd);
    free(meta_path_full);
    free(dir_path_meta);
    return -1;
  }

  // Write the metadata
  char *meta_content = headerToString(permission);
  ssize_t bytes_written_m = write(mfd, meta_content, strlen(meta_content));
  if (bytes_written_m == -1) {
    perror("Error writing meta");
    close(mfd);
    free(meta_content);
    free(meta_path_full);
    free(dir_path_meta);
    return -1;
  }

  // Release the lock on the meta file
  mlock.l_type = F_UNLCK;
  if (fcntl(mfd, F_SETLK, &mlock) == -1) {
    perror("fcntl unlock meta");
    close(mfd);
    free(meta_content);
    free(meta_path_full);
    free(dir_path_meta);
    return -1;
  }

  close(mfd);

  // free(meta_content);
  free(meta_path_full);
  free(dir_path_meta);

  // Construct the full file path by concatenating ROOT_DIR and file_path
  char *file_path_full = malloc(strlen(ROOT_DIR) + strlen(file_path) + 1);
  sprintf(file_path_full, "%s/%s", ROOT_DIR, file_path);

  // Parse the full file path into directory and filename
  char *dir_path_file = strdup(file_path_full);
  char *filename = strrchr(dir_path_file, '/');
  if (filename) {
    *filename++ = '\0';
  } else {
    filename = dir_path_file;
    dir_path_file = ".";
  }

  // Create the file directory if it doesn't exist
  if (mkdir(dir_path_file, 0755) != 0 && errno != EEXIST) {
    perror("mkdir file");
    free(dir_path_file);
    free(file_path_full);
    return -1;
  }

  // Open and lock the file
  int fd = open(file_path_full, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    perror("open file");
    free(dir_path_file);
    free(file_path_full);
    return -1;
  }

  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    perror("fcntl file");
    close(fd);
    free(dir_path_file);
    free(file_path_full);
    return -1;
  }

  // clean the buffer again
  memset(client_message, '\0', sizeof(client_message));
  memset(server_message, '\0', sizeof(client_message));

  // Read data from the client and write to the file
  if (recv(client_sockfd, client_message, BUFFER_SIZE, 0) < 0) {
    perror("Couldn't receive");
    close(fd);
    free(dir_path_file);
    free(file_path_full);
    return -1;
  }

  // encrypt client message
  char *encrypted_message = pbEncode(client_message);

  ssize_t bytes_written =
      write(fd, encrypted_message, strlen(encrypted_message));
  if (bytes_written < 0) {
    perror("Error writing to server file");
    close(fd);
    free(dir_path_file);
    free(file_path_full);
    return -1;
  }

  free(encrypted_message);

  // Release the lock on the file
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) == -1) {
    perror("fcntl unlock file");
    close(fd);
    free(dir_path_file);
    free(file_path_full);
  }

  close(fd);
  free(dir_path_file);
  free(file_path_full);

  // send success message to client
  sprintf(server_message, "Write Success\n");
  if (send(client_sockfd, server_message, strlen(server_message), 0) < 0) {
    printf("Can't send\n");
    return -1;
  }

  return 0;
}

/** return status code
 * 0 for success, -1 for failure
 */
int handle_get(int client_sockfd, char *file_path) {
  char *file_path_full = malloc(strlen(ROOT_DIR) + strlen(file_path) + 1);
  sprintf(file_path_full, "%s/%s", ROOT_DIR, file_path);

  int fd = open(file_path_full, O_RDONLY);
  if (fd < 0) {
    perror("open");
    free(file_path_full);
    return -1;
  }

  free(file_path_full);

  // read lock the file
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    perror("fcntl file");
    close(fd);
    return -1;
  }

  char buffer[BUFFER_SIZE];
  ssize_t read_bytes;

  memset(buffer, '\0', sizeof(buffer));

  if ((read_bytes = read(fd, buffer, BUFFER_SIZE)) > 0) {
    // decrypt the message
    char *decrypted_message = pbDecode(buffer);
    if (send(client_sockfd, decrypted_message, strlen(decrypted_message), 0) <
        0) {
      perror("send");
      close(fd);
      free(decrypted_message);
      return -1;
    }
    free(decrypted_message);
  } else {
    perror("read");
    return -1;
  }

  // Release the lock on the file
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLK, &lock) == -1) {
    perror("fcntl unlock file");
    close(fd);
  }

  close(fd);

  // receive acknowledgement from client
  char ack[BUFFER_SIZE];
  if (recv(client_sockfd, ack, BUFFER_SIZE, 0) < 0) {
    perror("Couldn't receive");
    return -1;
  }

  // send message to client
  char success_message[] = "Get Success\n";
  if (send(client_sockfd, success_message, strlen(success_message), 0) < 0) {
    perror("Couldn't send");
    return -1;
  }

  return 0;
}

/** return status code
 * 0 for success, -1 for failure
 */
int handle_rm(int client_sockfd, char *file_path) {
  char server_message[BUFFER_SIZE];
  memset(server_message, '\0', sizeof(server_message));

  // Construct the full meta file path
  char *meta_path_full = malloc(strlen(META_DIR) + strlen(file_path) + 1);
  sprintf(meta_path_full, "%s/%s", META_DIR, file_path);

  // Check permission
  if (getPermission(meta_path_full) == 0) {
    // The file already exists and the permission is READ ONLY
    free(meta_path_full);

    // Send failure message to client
    sprintf(server_message, "Can't remove: %s\nThis is a READ ONLY file\n",
            file_path);
    if (send(client_sockfd, server_message, strlen(server_message), 0) < 0) {
      printf("Can't send\n");
      return -1;
    }
    return 0;
  } else if (getPermission(meta_path_full) == -1) {
    // The file does not exist
    free(meta_path_full);

    // Send failure message to client
    sprintf(server_message, "Does not exist: %s\n", file_path);
    if (send(client_sockfd, server_message, strlen(server_message), 0) < 0) {
      printf("Can't send\n");
      return -1;
    }
    return 0;
  }

  // delete dir & meta on server
  // Open and lock the meta file
  int mfd = open(meta_path_full, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (mfd == -1) {
    perror("open meta");
    free(meta_path_full);
    return -1;
  }

  struct flock mlock;
  mlock.l_type = F_WRLCK;
  mlock.l_whence = SEEK_SET;
  mlock.l_start = 0;
  mlock.l_len = 0;

  if (fcntl(mfd, F_SETLKW, &mlock) == -1) {
    perror("fcntl meta");
    close(mfd);
    free(meta_path_full);
    return -1;
  }

  // Delete the metadata file
  if (unlink(meta_path_full) != 0) {
    perror("unlink meta");
    // No need to unlock the file, as it's already deleted
    close(mfd);
    free(meta_path_full);
    return -1;
  }

  free(meta_path_full);

  // delete dir & file on server
  // Construct the full file path by concatenating ROOT_DIR and file_path
  char *file_path_full = malloc(strlen(ROOT_DIR) + strlen(file_path) + 1);
  sprintf(file_path_full, "%s/%s", ROOT_DIR, file_path);

  // Open and lock the file
  int fd = open(file_path_full, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    perror("open file");
    free(file_path_full);
    return -1;
  }

  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    perror("fcntl file");
    close(fd);
    free(file_path_full);
    return -1;
  }

  // Delete the file
  if (unlink(file_path_full) != 0) {
    perror("unlink file");
    // No need to unlock the file, as it's already deleted
    close(fd);
    free(file_path_full);
    return -1;
  }

  free(file_path_full);

  // Send success message to client
  sprintf(server_message, "Remove Success\n");
  if (send(client_sockfd, server_message, strlen(server_message), 0) < 0) {
    printf("Can't send\n");
    return -1;
  }
  return 0;
}

/** return status code
 * 0 for success, -1 for failure
 */
int handle_ls(int client_sockfd, char *file_path) {
  // list info
  char *meta_path_full = malloc(strlen(META_DIR) + strlen(file_path) + 1);
  sprintf(meta_path_full, "%s/%s", META_DIR, file_path);

  int fd = open(meta_path_full, O_RDONLY);
  if (fd < 0) {

    // send message to client
    char fail_message[] = "No such file exist\n";
    if (send(client_sockfd, fail_message, strlen(fail_message), 0) < 0) {
      perror("Couldn't send");
      free(meta_path_full);
      return -1;
    }

    free(meta_path_full);
    return 0;
  }

  free(meta_path_full);

  char buffer[BUFFER_SIZE];
  ssize_t read_bytes;

  if ((read_bytes = read(fd, buffer, BUFFER_SIZE)) > 0) {
    if (send(client_sockfd, buffer, read_bytes, 0) < 0) {
      perror("send message over socket");
      close(fd);
      return -1;
    }
  } else {
    perror("read meta");
    return -1;
  }

  return 0;
}