CC=gcc
CFLAGS=-Wall -g

# Object files
CLIENT_OBJS=client.o clientHandler.o utils.o
SERVER_OBJS=server.o serverHandler.o parseHdr.o cipher.o utils.o

# Executable names
CLIENT_BIN=client
SERVER_BIN=server

all: $(CLIENT_BIN) $(SERVER_BIN) 

$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_OBJS)

$(SERVER_BIN): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_OBJS)

client.o: client.c clientHandler.h
	$(CC) $(CFLAGS) -c client.c

clientHandler.o: clientHandler.c clientHandler.h
	$(CC) $(CFLAGS) -c clientHandler.c

server.o: server.c serverHandler.h parseHdr.h
	$(CC) $(CFLAGS) -c server.c

serverHandler.o: serverHandler.c serverHandler.h
	$(CC) $(CFLAGS) -c serverHandler.c

parseHdr.o: parseHdr.c parseHdr.h
	$(CC) $(CFLAGS) -c parseHdr.c

cipher.o: cipher.c cipher.h
	$(CC) $(CFLAGS) -c cipher.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

clean:
	rm -f *.o $(CLIENT_BIN) $(SERVER_BIN)
