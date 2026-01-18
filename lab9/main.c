#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define BUFFER_CAPACITY 256

char data_buffer[BUFFER_CAPACITY];
int message_count = 0;
pthread_mutex_t buffer_lock;

void* consumer_function(void* arg) {
    (void)arg;
    
    printf("Consumer thread started (Thread ID: %p)\n", 
           (void*)pthread_self());

    while (1) {
        pthread_mutex_lock(&buffer_lock);

        time_t current = time(NULL);
        struct tm* time_data = localtime(&current);
        char time_str[30];
        strftime(time_str, sizeof(time_str), 
                 "%H:%M:%S", time_data);

        printf("Consumer (ID: %p) | Time: %s | Buffer: %s\n",
               (void*)pthread_self(), time_str, data_buffer);

        pthread_mutex_unlock(&buffer_lock);
        sleep(2);
    }

    return NULL;
}

void* producer_function(void* arg) {
    (void)arg; 
    
    printf("Producer thread started (Thread ID: %p)\n", 
           (void*)pthread_self());

    while (1) {
        pthread_mutex_lock(&buffer_lock);

        message_count++;
        time_t current = time(NULL);
        struct tm* time_data = localtime(&current);

        snprintf(data_buffer, BUFFER_CAPACITY,
                 "Message #%d at %02d:%02d:%02d",
                 message_count,
                 time_data->tm_hour,
                 time_data->tm_min,
                 time_data->tm_sec);

        printf("Producer (ID: %p): Created - %s\n",
               (void*)pthread_self(), data_buffer);

        pthread_mutex_unlock(&buffer_lock);
        sleep(1);
    }

    return NULL;
}

void start_application() {
    pthread_t producer_thread, consumer_thread;

    strcpy(data_buffer, "Initial buffer state");

    if (pthread_mutex_init(&buffer_lock, NULL) != 0) {
        fprintf(stderr, "Mutex initialization failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&producer_thread, NULL, producer_function, NULL) != 0) {
        fprintf(stderr, "Producer thread creation failed\n");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&consumer_thread, NULL, consumer_function, NULL) != 0) {
        fprintf(stderr, "Consumer thread creation failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Application running. Press Ctrl+C to stop.\n");

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&buffer_lock);
}

int main() {
    start_application();
    return EXIT_SUCCESS;
}