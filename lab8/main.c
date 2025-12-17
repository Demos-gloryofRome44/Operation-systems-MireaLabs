#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "messages.h"

char shared_array[ARRAY_SIZE];
pthread_mutex_t mutex;
int write_counter = 0;
volatile int program_running = 1;

void* writer_thread(void* arg) {
    (void)arg;
    
    printf("%s\n", MSG_WRITER_STARTED);
    
    while (program_running) {
        pthread_mutex_lock(&mutex);
        
        printf("%s\n", MSG_WRITER_WRITING);
        
        write_counter++;
        snprintf(shared_array, ARRAY_SIZE, "Запись %d", write_counter);
        
        printf(MSG_WRITER_WROTE, shared_array);
        printf("\n");
        
        pthread_mutex_unlock(&mutex);
        
        sleep(WRITER_SLEEP_TIME);
    }
    
    printf("%s\n", "Пишущий поток завершает работу");
    return NULL;
}

void* reader_thread(void* arg) {
    long thread_id = (long)arg;
    
    printf(MSG_READER_STARTED, thread_id);
    printf("\n");
    
    while (program_running) {
        pthread_mutex_lock(&mutex);
        
        printf(MSG_READER_READ, thread_id, (unsigned long)pthread_self(), shared_array);
        printf("\n");
        
        pthread_mutex_unlock(&mutex);
        
        sleep(READER_SLEEP_TIME);
    }
    
    printf("Читающий поток %ld завершает работу\n", thread_id);
    return NULL;
}

int main() {
    pthread_t writer_tid;
    pthread_t reader_tids[NUM_READERS];
    int i;
    
    printf("%s\n", MSG_PROGRAM_START);
    printf(MSG_THREADS_CREATED, NUM_READERS);
    printf("\n");
    
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "%s\n", MSG_MUTEX_INIT_ERROR);
        exit(EXIT_FAILURE);
    }
    
    strcpy(shared_array, "Начальное значение");
    
    if (pthread_create(&writer_tid, NULL, writer_thread, NULL) != 0) {
        fprintf(stderr, "%s\n", MSG_THREAD_CREATE_ERROR);
        pthread_mutex_destroy(&mutex);
        exit(EXIT_FAILURE);
    }
    
    for (i = 0; i < NUM_READERS; i++) {
        if (pthread_create(&reader_tids[i], NULL, reader_thread, 
                          (void*)(long)(i + 1)) != 0) {
            fprintf(stderr, "%s %d\n", MSG_THREAD_CREATE_ERROR, i + 1);
            
            program_running = 0;
            
            for (int j = 0; j < i; j++) {
                pthread_join(reader_tids[j], NULL);
            }
            pthread_join(writer_tid, NULL);
            
            pthread_mutex_destroy(&mutex);
            exit(EXIT_FAILURE);
        }
    }
    
    printf(MSG_PROGRAM_WORKING, PROGRAM_DURATION);
    printf("\n");
    
    sleep(PROGRAM_DURATION);
    
    printf("\n%s\n", MSG_PROGRAM_END);
    program_running = 0;
    
    printf("%s\n", MSG_WAITING_THREADS);
    
    for (i = 0; i < NUM_READERS; i++) {
        if (pthread_join(reader_tids[i], NULL) != 0) {
            fprintf(stderr, "%s: читающего потока %d\n", MSG_THREAD_JOIN_ERROR, i + 1);
        }
    }
    
    if (pthread_join(writer_tid, NULL) != 0) {
        fprintf(stderr, "%s: пишущего потока\n", MSG_THREAD_JOIN_ERROR);
    }
    
    if (pthread_mutex_destroy(&mutex) != 0) {
        fprintf(stderr, "%s\n", MSG_MUTEX_DESTROY_ERROR);
    }
    
    printf("\n%s\n", MSG_PROGRAM_FINISH);
    printf(MSG_TOTAL_WRITES, write_counter);
    printf("\n");
    
    return EXIT_SUCCESS;
}