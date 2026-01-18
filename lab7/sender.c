#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include "messages.h"

typedef struct {
    char timestamp[64];
    int process_id;
    int operation_count;
} SharedData;

static SharedData* shared_memory = NULL;
static int shared_memory_id = -1;

void release_resources(int signal_number) {
    (void)signal_number;
    
    printf(MSG_SENDER_EXIT);
    printf(MSG_CLEANUP_RESOURCES);
    
    if (shared_memory != NULL && shared_memory != (void*)-1) {
        if (shmdt(shared_memory) == -1) {
            perror("Ошибка отсоединения разделяемой памяти");
        }
    }
    
    if (shared_memory_id != -1) {
        if (shmctl(shared_memory_id, IPC_RMID, NULL) == -1) {
            perror("Ошибка удаления разделяемой памяти");
        }
    }
    
    remove(SHM_KEY_FILE);
    
    exit(EXIT_SUCCESS);
}

int create_shared_memory() {
    FILE* key_file = fopen(SHM_KEY_FILE, "w");
    if (key_file == NULL) {
        perror("Ошибка создания ключевого файла");
        return -1;
    }
    fclose(key_file);
    
    key_t memory_key = ftok(SHM_KEY_FILE, SHM_KEY_ID);
    if (memory_key == -1) {
        perror("Ошибка генерации ключа");
        remove(SHM_KEY_FILE);
        return -1;
    }
    
    shared_memory_id = shmget(memory_key, sizeof(SharedData), 
                              IPC_CREAT | IPC_EXCL | 0666);
    if (shared_memory_id == -1) {
        if (errno == EEXIST) {
            fprintf(stderr, MSG_ALREADY_RUNNING);
        } else {
            perror("Ошибка создания разделяемой памяти");
        }
        remove(SHM_KEY_FILE);
        return -1;
    }
    
    shared_memory = (SharedData*)shmat(shared_memory_id, NULL, 0);
    if (shared_memory == (void*)-1) {
        perror("Ошибка присоединения к разделяемой памяти");
        shmctl(shared_memory_id, IPC_RMID, NULL);
        remove(SHM_KEY_FILE);
        return -1;
    }
    
    shared_memory->process_id = getpid();
    shared_memory->operation_count = 0;
    strcpy(shared_memory->timestamp, "Инициализация");
    
    printf(MSG_SHARED_MEM_CREATED);
    return 0;
}

int main(void) {
    signal(SIGINT, release_resources);
    signal(SIGTERM, release_resources);
    
    if (create_shared_memory() != 0) {
        return EXIT_FAILURE;
    }
    
    printf(MSG_SENDER_START, getpid());
    printf(MSG_CTRL_C_HINT);
    
    int operations_performed = 0;
    
    while (operations_performed < MAX_OPERATIONS) {
        printf(MSG_WRITING_DATA);
        
        time_t current_time;
        struct tm* time_info;
        
        time(&current_time);
        time_info = localtime(&current_time);
        
        strftime(shared_memory->timestamp, sizeof(shared_memory->timestamp),
                TIME_FORMAT_LONG, time_info);
        
        shared_memory->process_id = getpid();
        shared_memory->operation_count++;
        operations_performed++;
        
        printf(MSG_DATA_WRITTEN, shared_memory->timestamp, shared_memory->process_id);
        
        sleep(1);
    }
    
    printf(MSG_OPERATIONS_COMPLETED, operations_performed);
    release_resources(0);
    
    return EXIT_SUCCESS;
}