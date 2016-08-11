CC=gcc
CFLAGS=-Wall -g -O2 -c

all:server client

server:server.o
	$(CC) $< -o $@

client:client.o
	$(CC) $< -o $@

%*.o:%*.c
	$(CC) $(CFLAGS) $< -o $@

.PHONY:
	clean

clean:
	rm *.o server client

