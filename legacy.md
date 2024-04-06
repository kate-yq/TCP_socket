### legacy server.c main()  
     
// legacy main function
int main() {
    int server_sockfd, client_sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    pthread_t thread_id;

    // Create socket
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the specified IP and port
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sockfd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        addr_len = sizeof(client_addr);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sockfd < 0) {
            perror("accept");
            continue;
        }

        int *client_sockfd_ptr = malloc(sizeof(int));
        *client_sockfd_ptr = client_sockfd;

        // Create a new thread to handle the client
        if (pthread_create(&thread_id, NULL, handle_client, (void *)client_sockfd_ptr) < 0) {
            perror("pthread_create");
            close(client_sockfd);
            free(client_sockfd_ptr);
            continue;
        }

        // Detach the thread to prevent memory leaks
        pthread_detach(thread_id);
    }

    return 0;
}

### legacy client main()
int main(int arg, char **args) {
  int socket_desc;
  struct sockaddr_in server_addr;
  char server_message[2000], client_message[2000];

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));

  // client message = command line arguements
  // ./client rfs COMMAND local (remote)
  if (arg >= 3 && arg <= 6) {
    char *commandResult = commandToString(args, arg);
    if (commandResult != NULL) {
      sprintf(client_message, "%s", commandResult);
      free(commandResult);
    } else {
      // Handle memory allocation error
      printf("Error: Unable to generate command string\n");
      return -1;
    }
  } else {
    // num of args is not correct
    printf("ERROR: arguements are not in correct format\n");
    return 1;
  }

  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  if (socket_desc < 0) {
    printf("Unable to create socket\n");
    close(socket_desc);
    return -1;
  }

  printf("Socket created successfully\n");

  // Set port and IP the same as server-side:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Send connection request to server:
  if (connect(socket_desc, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    printf("Unable to connect\n");
    close(socket_desc);
    return -1;
  }
  printf("Connected with server successfully\n");

  // Send the message to server:
  if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
    printf("Unable to send message\n");
    close(socket_desc);
    return -1;
  }

  // Receive the server's response:
  if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
    printf("Error while receiving server's msg\n");
    close(socket_desc);
    return -1;
  }

  printf("Server's response: %s\n", server_message);

  // Close the socket:
  close(socket_desc);

  return 0;
}
