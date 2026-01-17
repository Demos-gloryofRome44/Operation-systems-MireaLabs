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
pthread_mutex_t mutex;
pthread_cond_t cond_read;  
pthread_cond_t cond_write; 

int write_counter = 0;
int running = 1;
int active_readers = 0;    
int writer_waiting = 0;    
int writer_active = 0;     

void* reader_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    
    while (running) {
        pthread_mutex_lock(&mutex);
        
        while (writer_active || writer_waiting) {
            pthread_cond_wait(&cond_read, &mutex);
        }
        
        active_readers++;
        
        pthread_mutex_unlock(&mutex);
        
        printf(MSG_READER_FORMAT, 
               data->thread_id, (void*)data->tid, shared_buffer, active_readers);
        
        pthread_mutex_lock(&mutex);
        active_readers--;
        
        if (active_readers == 0 && writer_waiting) {
            pthread_cond_signal(&cond_write);
        }
        
        pthread_mutex_unlock(&mutex);
        
        usleep(100000 + rand() % 200000); 
    }
    
    printf(MSG_READER_FINISHED, data->thread_id);
    pthread_exit(NULL);
}

void* writer_thread(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int operations = 0;
    
    while (operations < MAX_OPERATIONS) {
        pthread_mutex_lock(&mutex);
        
        while (active_readers > 0 || writer_active) {
            writer_waiting = 1;
            pthread_cond_wait(&cond_write, &mutex);
            writer_waiting = 0;
        }
        
        writer_active = 1;
        
        pthread_mutex_unlock(&mutex);
        
        write_counter++;
        snprintf(shared_buffer, BUFFER_SIZE, 
                MSG_WRITE_FORMAT, write_counter, (void*)data->tid);
        
        printf(MSG_WRITER_WRITE, (void*)data->tid, shared_buffer);
        
        pthread_mutex_lock(&mutex);
        writer_active = 0;
        operations++;
        
        if (writer_waiting) {
            pthread_cond_signal(&cond_write);
        } else {
            pthread_cond_broadcast(&cond_read);
        }
        
        pthread_mutex_unlock(&mutex);
        
        usleep(500000); 
    }
    
    pthread_mutex_lock(&mutex);
    running = 0;
    pthread_cond_broadcast(&cond_read);
    pthread_cond_broadcast(&cond_write);
    pthread_mutex_unlock(&mutex);
    
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
    
    rc = pthread_mutex_init(&mutex, NULL);
    if (rc) {
        printf(MSG_MUTEX_INIT_ERROR, rc);
        return 1;
    }
    
    rc = pthread_cond_init(&cond_read, NULL);
    if (rc) {
        printf(MSG_COND_INIT_ERROR, rc);
        pthread_mutex_destroy(&mutex);
        return 1;
    }
    
    rc = pthread_cond_init(&cond_write, NULL);
    if (rc) {
        printf(MSG_COND_INIT_ERROR, rc);
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_read);
        return 1;
    }
    
    for (i = 0; i < READER_COUNT; i++) {
        reader_data[i].thread_id = i + 1;
        rc = pthread_create(&readers[i], NULL, reader_thread, &reader_data[i]);
        if (rc) {
            printf(MSG_READER_CREATE_ERROR, i, rc);
            pthread_mutex_destroy(&mutex);
            pthread_cond_destroy(&cond_read);
            pthread_cond_destroy(&cond_write);
            return 1;
        }
        reader_data[i].tid = readers[i];
    }
    
    writer_data.thread_id = 1;
    rc = pthread_create(&writer, NULL, writer_thread, &writer_data);
    if (rc) {
        printf(MSG_WRITER_CREATE_ERROR, rc);
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond_read);
        pthread_cond_destroy(&cond_write);
        return 1;
    }
    writer_data.tid = writer;
    
    pthread_join(writer, NULL);
    
    sleep(1);
    
    for (i = 0; i < READER_COUNT; i++) {
        pthread_cancel(readers[i]);
        pthread_join(readers[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_read);
    pthread_cond_destroy(&cond_write);
    
    printf(MSG_PROGRAM_FINISHED);
    printf(MSG_TOTAL_WRITES, write_counter);
    
    return 0;
}