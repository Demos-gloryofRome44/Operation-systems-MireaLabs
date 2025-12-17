CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS =

.PHONY: all clean sender receiver run_sender run_receiver

all: sender receiver

sender: sender.c
	$(CC) $(CFLAGS) sender.c -o sender $(LDFLAGS)

receiver: receiver.c
	$(CC) $(CFLAGS) receiver.c -o receiver $(LDFLAGS)

clean:
	rm -f sender receiver
	rm -f /tmp/.time-transfer-info

run_sender: sender
	./sender

run_receiver: receiver
	./receiver