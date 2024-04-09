# Remote File System over TCP Socket

This project implements a remote file system in C programming language. It provides various functions to handle file operations such as writing from a local file to a remote file, reading from a remote file, removing a file, and listing file permission. The project consists of server-side and client-side. The server accepts commands from clients and executes corresponding operations in a multithreaded behavior.

## Components:

1. Multithreaded TCP Socket Server
2. TCP Socket Client
3. Directories:

- Local directory: "local"
- Remote directory: "file"
- Permission directory: "meta"

4. Test script: "test_rfs.sh"

## Features

- **Support for Multiple Clients:** The server can handle multiple client connections concurrently using threads.
- **File Operations:** Supports file WRITE, GET, RM, LS, STOP, and HELP operations.
- **Flexible Command Handling:** Clients can send commands with arguments to perform different file operations. Invalid instructions will be handled properly, thus will NOT affect the server running.
- **Graceful Shutdown:** The server can be stopped gracefully by sending a "STOP" command.
- **Cipher and Decipher**: As required, the file will be ciphered before written to server's disk, and deciphered before sent to client side.
- **Optional input:** the second directory is default to be the same as the fist one, unless specifically state.

## How to run

There are two ways to run the program.

### Compile the project and run

Running this project will require 2 terminal windows.

1. Server side:  
   `make`  
   `./server`
2. After the server starts running, client side:  
    `./client rfs HELP`  
   to see all the available commands.

### Run everything using a test script

1. Make the script executable:  
   `chmod +x test_rfs.sh`
2. Run the script:  
   `./test_rfs.sh`
