#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "messages.h"

int main() {
    int file_descriptor;
    char transmission_buffer[BUFFER_SIZE];
    char timestamp_buffer[BUFFER_SIZE];
    
    if (access(FIFO_NAME, F_OK) == -1) {
        mkfifo(FIFO_NAME, 0666);
    }
    
    printf(SENDER_STARTED, getpid());
    
    file_descriptor = open(FIFO_NAME, O_WRONLY);
    
    get_current_time_str(timestamp_buffer, BUFFER_SIZE);
    format_message(transmission_buffer, "Данные от отправителя", getpid(), timestamp_buffer);
    
    printf("Отправляем: %s\n", transmission_buffer);
    
    write(file_descriptor, transmission_buffer, strlen(transmission_buffer) + 1);
    close(file_descriptor);
    
    printf(SENDER_COMPLETED);
    
    sleep(2);
    
    unlink(FIFO_NAME);
    
    return 0;
}