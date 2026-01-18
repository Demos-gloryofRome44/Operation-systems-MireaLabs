#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "messages.h"

typedef struct {
    pthread_t tid;
    int thread_id;
} thread_data_t;

char shared_buffer[BUFFER_SIZE];
pthread_rwlock_t rwlock;
int write_counter = 0;
int running = 1;

void* reader_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    
    while (running) {
        pthread_rwlock_rdlock(&rwlock);
        
        printf(MSG_READER_FORMAT, 
               data->thread_id, (void*)data->tid, shared_buffer);
        
        pthread_rwlock_unlock(&rwlock);
        
        usleep(100000 + rand() % 200000); 
    }
    
    printf(MSG_READER_FINISHED, data->thread_id);
    pthread_exit(NULL);
}

void* writer_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int operations = 0;
    
    while (operations < MAX_OPERATIONS) {
        pthread_rwlock_wrlock(&rwlock);
        
        write_counter++;
        snprintf(shared_buffer, BUFFER_SIZE, 
                MSG_WRITE_FORMAT, write_counter, (void*)data->tid);
        
        printf(MSG_WRITER_WRITE, (void*)data->tid, shared_buffer);
        
        pthread_rwlock_unlock(&rwlock);
        
        operations++;
        
        usleep(500000); 
    }
    
    running = 0;
    
    printf(MSG_WRITER_FINISHED, operations);
    pthread_exit(NULL);
}

int main() {
    pthread_t readers[READER_COUNT];
    pthread_t writer;
    thread_data_t reader_data[READER_COUNT];
    thread_data_t writer_data;
    int i, rc;
    
    printf(MSG_READERS_COUNT, READER_COUNT);
    printf(MSG_WRITERS_COUNT, WRITER_COUNT);
    printf(MSG_MAX_OPERATIONS, MAX_OPERATIONS);
    
    strcpy(shared_buffer, MSG_INIT_BUFFER);
    
    rc = pthread_rwlock_init(&rwlock, NULL);
    if (rc) {
        printf(MSG_RWLOCK_INIT_ERROR, rc);
        return 1;
    }
    
    for (i = 0; i < READER_COUNT; i++) {
        reader_data[i].thread_id = i + 1;
        rc = pthread_create(&readers[i], NULL, reader_thread, &reader_data[i]);
        if (rc) {
            printf(MSG_READER_CREATE_ERROR, i, rc);
            return 1;
        }
        reader_data[i].tid = readers[i];
    }
    
    writer_data.thread_id = 1;
    rc = pthread_create(&writer, NULL, writer_thread, &writer_data);
    if (rc) {
        printf(MSG_WRITER_CREATE_ERROR, rc);
        return 1;
    }
    writer_data.tid = writer;
    
    pthread_join(writer, NULL);
    
    sleep(1);
    
    for (i = 0; i < READER_COUNT; i++) {
        pthread_cancel(readers[i]);
        pthread_join(readers[i], NULL);
    }
    
    pthread_rwlock_destroy(&rwlock);
    
    printf(MSG_PROGRAM_FINISHED);
    printf(MSG_TOTAL_WRITES, write_counter);
    
    return 0;
}