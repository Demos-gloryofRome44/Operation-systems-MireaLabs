#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include "messages.h"

int main() {
    int pipe_descriptors[2];
    pid_t process_id;
    char data_buffer[BUFFER_SIZE];
    char time_buffer[BUFFER_SIZE];
    char message_buffer[BUFFER_SIZE];
    
    if (pipe(pipe_descriptors) == -1) {
        perror(PIPE_ERROR_CREATE_PIPE);
        exit(EXIT_FAILURE);
    }
    
    process_id = fork();
    
    if (process_id == -1) {
        perror(PIPE_ERROR_CREATE_PROCESS);
        exit(EXIT_FAILURE);
    }
    
    if (process_id > 0) { 
        close(pipe_descriptors[0]); 
        
        get_current_time_str(time_buffer, BUFFER_SIZE);
        format_message(message_buffer, "Сообщение от родителя", getpid(), time_buffer);
        
        write(pipe_descriptors[1], message_buffer, strlen(message_buffer) + 1);
        
        printf(PIPE_PARENT_SENDING, getpid());
        
        close(pipe_descriptors[1]);
        
        wait(NULL);
        
    } else { 
        close(pipe_descriptors[1]);
        
        sleep(PIPE_SLEEP_TIME);
        
        read(pipe_descriptors[0], data_buffer, BUFFER_SIZE);
        
        get_current_time_str(time_buffer, BUFFER_SIZE);
        printf("\n");
        printf(PIPE_CHILD_HEADER, getpid());
        printf("\n");
        printf(PROCESS_MESSAGE, data_buffer);
        printf(TIME_FORMAT, time_buffer);
        
        close(pipe_descriptors[0]);
    }
    
    return 0;
}