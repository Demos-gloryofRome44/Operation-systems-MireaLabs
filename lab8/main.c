#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "messages.h"

char shared_array[ARRAY_SIZE];
pthread_mutex_t mutex;
int write_counter = 0;

void* writer_thread(void* arg) {
    printf("%s\n", MSG_WRITER_STARTED);
    
    while (1) {
        pthread_mutex_lock(&mutex);
        
        printf("%s\n", MSG_WRITER_WRITING);
        
        write_counter++;
        snprintf(shared_array, ARRAY_SIZE, "Запись %d", write_counter);
        
        printf(MSG_WRITER_WROTE, shared_array);
        printf("\n");
        
        pthread_mutex_unlock(&mutex);
        
        sleep(WRITER_SLEEP_TIME);
    }
    
    return NULL;
}

void* reader_thread(void* arg) {
    long thread_id = (long)arg;
    
    printf(MSG_READER_STARTED, thread_id);
    printf("\n");
    
    while (1) {
        pthread_mutex_lock(&mutex);
        
        printf(MSG_READER_READ, thread_id, shared_array);
        printf("\n");
        
        pthread_mutex_unlock(&mutex);
        
        sleep(READER_SLEEP_TIME);
    }
    
    return NULL;
}

void cleanup_thread(pthread_t thread, const char* thread_type) {
    int cancel_result = pthread_cancel(thread);
    if (cancel_result != 0) {
        fprintf(stderr, "%s: %s\n", MSG_THREAD_CANCEL_ERROR, thread_type);
    }
}

int main() {
    pthread_t writer_tid;
    pthread_t reader_tids[NUM_READERS];
    int i;
    
    printf("%s\n", MSG_PROGRAM_STARTED);
    
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
        if (pthread_create(&reader_tids[i], NULL, reader_thread, (void*)(long)(i + 1)) != 0) {
            fprintf(stderr, "%s %d\n", MSG_THREAD_CREATE_ERROR, i + 1);
            
            cleanup_thread(writer_tid, "пишущего потока");
            for (int j = 0; j < i; j++) {
                cleanup_thread(reader_tids[j], "читающего потока");
            }
            
            pthread_mutex_destroy(&mutex);
            exit(EXIT_FAILURE);
        }
    }
    
    printf("Программа будет работать %d секунд...\n", PROGRAM_DURATION);
    
    sleep(PROGRAM_DURATION);
    
    printf("\n%s\n", MSG_PROGRAM_ENDING);
    
    cleanup_thread(writer_tid, "пишущего потока");
    
    for (i = 0; i < NUM_READERS; i++) {
        cleanup_thread(reader_tids[i], "читающего потока");
    }
    
    printf("%s\n", MSG_THREAD_JOINED);
    
    if (pthread_join(writer_tid, NULL) != 0) {
        fprintf(stderr, "%s: пишущего потока\n", MSG_THREAD_JOIN_ERROR);
    }
    
    for (i = 0; i < NUM_READERS; i++) {
        if (pthread_join(reader_tids[i], NULL) != 0) {
            fprintf(stderr, "%s: читающего потока %d\n", MSG_THREAD_JOIN_ERROR, i + 1);
        }
    }
    
    if (pthread_mutex_destroy(&mutex) != 0) {
        fprintf(stderr, "%s\n", MSG_MUTEX_DESTROY_ERROR);
    }
    
    printf("\n%s\n", MSG_PROGRAM_FINISHED);
    printf("Всего было сделано записей: %d\n", write_counter);
    
    return 0;
}