#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "messages.h"

typedef struct {
    char data_buffer[BUFFER_CAPACITY];
    int counter_value;
    sem_t access_control;
} SharedDataContainer;

SharedDataContainer shared_container;

void initialize_resources() {
    strcpy(shared_container.data_buffer, THREADS_INITIAL_BUFFER_CONTENT);
    shared_container.counter_value = 0;
    
    if (sem_init(&shared_container.access_control, 0, 1) != 0) {
        perror(ERROR_SEMAPHORE_INIT);
        exit(EXIT_FAILURE);
    }
}

void cleanup_resources() {
    sem_destroy(&shared_container.access_control);
}

void write_to_buffer() {
    shared_container.counter_value++;
    sprintf(shared_container.data_buffer, "%d", shared_container.counter_value);
}

void read_from_buffer(char* output) {
    strcpy(output, shared_container.data_buffer);
}

void* writer_thread_executor() {
    printf(THREADS_WRITER_START, pthread_self());

    while (1) {
        sem_wait(&shared_container.access_control);
        
        time_t current = time(NULL);
        struct tm* time_info = localtime(&current);
        write_to_buffer();
        
        char temp_buffer[MAX_MESSAGE_LENGTH];
        snprintf(temp_buffer, sizeof(temp_buffer),
                 IPC_SENDER_MESSAGE_FORMAT,
                 shared_container.counter_value,
                 time_info->tm_hour,
                 time_info->tm_min,
                 time_info->tm_sec);
        
        printf(THREADS_WRITER_LOG, pthread_self(), temp_buffer);
        
        sem_post(&shared_container.access_control);
        sleep(1);
    }

    return NULL;
}

void* reader_thread_executor() {
    printf(THREADS_READER_START, pthread_self());

    while (1) {
        sem_wait(&shared_container.access_control);
        
        char buffer_content[BUFFER_CAPACITY];
        read_from_buffer(buffer_content);
        
        time_t current = time(NULL);
        struct tm* time_info = localtime(&current);
        char time_string[TIME_FORMAT_BUFFER];
        strftime(time_string, sizeof(time_string), "%H:%M:%S", time_info);
        
        printf(THREADS_READER_LOG, pthread_self(), time_string, buffer_content);
        
        sem_post(&shared_container.access_control);
        sleep(2);
    }

    return NULL;
}

int main() {
    pthread_t writer_id, reader_id;

    initialize_resources();

    if (pthread_create(&writer_id, NULL, writer_thread_executor, NULL) != 0) {
        perror(ERROR_THREAD_CREATE);
        cleanup_resources();
        return EXIT_FAILURE;
    }

    if (pthread_create(&reader_id, NULL, reader_thread_executor, NULL) != 0) {
        perror(ERROR_THREAD_CREATE);
        cleanup_resources();
        return EXIT_FAILURE;
    }

    printf("%s\n", THREADS_MAIN_START);

    pthread_join(writer_id, NULL);
    pthread_join(reader_id, NULL);

    cleanup_resources();
    return EXIT_SUCCESS;
}