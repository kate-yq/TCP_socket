CC=gcc
CFLAGS=-Wall -g

# Object files
CLIENT_OBJS=client.o clientHandler.o
SERVER_OBJS=server.o serverHandler.o parseHdr.o cipher.o

# Executable names
CLIENT_BIN=client
SERVER_BIN=server
EVALUATION_BIN=evaluation

all: $(CLIENT_BIN) $(SERVER_BIN) $(EVALUATION_BIN)

$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_OBJS)

$(SERVER_BIN): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_OBJS)

evaluation.o: evaluation.c
	$(CC) $(CFLAGS) -c evaluation.c

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

$(EVALUATION_BIN): evaluation.o
	$(CC) $(CFLAGS) -o $(EVALUATION_BIN) evaluation.o

clean:
	rm -f *.o $(CLIENT_BIN) $(SERVER_BIN) $(EVALUATION_BIN)
