#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "messages.h"

int main() {
    int file_descriptor;
    char received_data[BUFFER_SIZE];
    char current_timestamp[BUFFER_SIZE];
    time_t start_time, end_time;
    
    printf(RECEIVER_STARTED, getpid());
    
    if (access(FIFO_NAME, F_OK) == -1) {
        mkfifo(FIFO_NAME, 0666);
        printf("Создан FIFO %s\n", FIFO_NAME);
    }
    
    printf("Ждем открытия FIFO для записи...\n");
    file_descriptor = open(FIFO_NAME, O_RDONLY);
    if (file_descriptor == -1) {
        perror(ERROR_MESSAGE);
        return 1;
    }
    
    printf("Читаем данные из FIFO...\n");
    start_time = time(NULL);
    ssize_t bytes_read = read(file_descriptor, received_data, BUFFER_SIZE);
    close(file_descriptor);
    
    if (bytes_read <= 0) {
        printf(ERROR_MESSAGE);
        return 1;
    }
    
    sleep(FIFO_SLEEP_TIME); 
    get_current_time_str(current_timestamp, BUFFER_SIZE);
    end_time = time(NULL);
    
    printf("\n");
    printf(RECEIVER_HEADER, getpid());
    printf("\n");
    printf(PROCESS_MESSAGE, received_data);
    printf(TIME_FORMAT, current_timestamp);
    printf("Фактическая задержка: %ld секунд\n", (end_time - start_time));
    
    unlink(FIFO_NAME);
    
    return 0;
}