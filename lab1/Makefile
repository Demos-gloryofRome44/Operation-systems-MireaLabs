CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGETS = mycat mygrep

.PHONY: all clean

all: $(TARGETS)

mycat: demcat.c
	$(CC) $(CFLAGS) -o mycat demcat.c

mygrep: demgrep.c
	$(CC) $(CFLAGS) -o mygrep demgrep.c

clean:
	rm -f $(TARGETS)