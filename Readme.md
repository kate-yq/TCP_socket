# TODO:
1. why server stopped when error


## Design

- Q1 WRITE

`% rfs WRITE local-file-path remote-file-path ` (need set system's PATH variable) 

or `% ./client rfs WRITE local-file-path remote-file-path`

or `% ./rfs WRITE local-file-path remote-file-path`

How to store Metadata? - mirror a directory for metadata


- Q2 GET

reversed version of WRITE

function1: read + slice (may need to write in client/server directly)

function2: create file + append 

while (1){
  if (!fileToBuffer(*bufferpointer, *filepointer, xx)){
    break;
  }
  bufferTofile(*bufferpointer, *filepointer, xx);
}



- Q3 RM remote content

remove file and matadata

return messages:

"success" / "fail" / "no such file"


- Q4 Multithread

mutex lock?? - need other struct, like File_t?

flock() ? 

Queue to store threads / client requests?


- Q5 Access Permission

int in metadata? 0/1/2/...


- Q6 Cipher

cipher before store in remote
decipher before get from remote


- Q7 LS

check metadata
format? name, dir, access permission, last modified time...

- Additional: Close Server   DONE

a CLI from client - STOP


# Question to ask

### Server buffer: only 1 or 1 for each client?

buffer in threads, each thread handles a client file descriptor. Use this to communicate. 

### If too many client connection requests, what to store to delay the connection? (message queue)

don't need to worry about this. threads need much less resources than process. Just accept connections and create thread.

### When the thread is created in server?

After accepting client connection (descriptor created), and pass the descriptor to the thread

### Is mirroring file system allowed?

### single client act and disconnect, or keep acting until done?

In other words, how can we demo the concurrent use?

### monitor / flock()

- fcntl: example

```c
#include <fcntl.h>
#include <unistd.h>

// Acquire an exclusive lock for writing
int fd = open("file.txt", O_RDWR);
struct flock lock;
lock.l_type = F_WRLCK;
lock.l_whence = SEEK_SET;
lock.l_start = 0;
lock.l_len = 0;  // Lock the entire file
fcntl(fd, F_SETLKW, &lock);

// Write to the file
write(fd, data, data_len);

// Release the exclusive lock
lock.l_type = F_UNLCK;
fcntl(fd, F_SETLK, &lock);
close(fd);
```
