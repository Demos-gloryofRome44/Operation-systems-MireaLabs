CC = gcc
CFLAGS = -Wall -Wextra -O2

all: sender receiver

sender: sender.c messages.h
	$(CC) $(CFLAGS) sender.c -o sender

receiver: receiver.c messages.h
	$(CC) $(CFLAGS) receiver.c -o receiver

clean:
	rm -f sender receiver

.PHONY: all clean