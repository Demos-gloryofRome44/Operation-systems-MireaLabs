CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGETS = demcat demgrep

.PHONY: all clean

all: $(TARGETS)

demcat: demcat.c
	$(CC) $(CFLAGS) -o demcat demcat.c

demgrep: demgrep.c
	$(CC) $(CFLAGS) -o demgrep demgrep.c

clean:
	rm -f $(TARGETS)