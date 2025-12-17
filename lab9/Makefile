CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -D_XOPEN_SOURCE=700
LDFLAGS = -pthread

TARGETS = main sender receiver

all: $(TARGETS)

pthread_example: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $<

receiver: receiver.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS) shm.key

.PHONY: all clean distclean