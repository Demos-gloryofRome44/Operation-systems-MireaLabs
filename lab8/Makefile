CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = main

all: $(TARGET)

$(TARGET): main.c messages.h
	$(CC) $(CFLAGS) main.c -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run