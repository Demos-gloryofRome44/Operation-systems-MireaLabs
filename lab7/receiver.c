#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include "messages.h"

typedef struct {
    char time_str[64];
    int pid;
} shared_data_t;

shared_data_t *shared_data = NULL;
int shm_fd = -1;
sem_t *sem_sender = NULL;
sem_t *sem_receiver = NULL;

void cleanup() {
    if (shared_data != MAP_FAILED && shared_data != NULL) {
        if (munmap(shared_data, sizeof(shared_data_t)) == -1) {
            perror(MSG_SHM_DETACH_ERROR);
        }
    }
    
    if (shm_fd != -1) {
        close(shm_fd);
    }
    
    if (sem_sender != SEM_FAILED && sem_sender != NULL) {
        if (sem_close(sem_sender) == -1) {
            perror(MSG_SEM_CLOSE_ERROR);
        }
    }
    
    if (sem_receiver != SEM_FAILED && sem_receiver != NULL) {
        if (sem_close(sem_receiver) == -1) {
            perror(MSG_SEM_CLOSE_ERROR);
        }
    }
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        cleanup();
        exit(0);
    }
}

int main() {
    signal(SIGINT, signal_handler);
    
    sem_sender = sem_open(SEM_SENDER_NAME, 0);
    if (sem_sender == SEM_FAILED) {
        perror(MSG_SEM_OPEN_ERROR);
        printf("%s\n", MSG_IPC_FAILED);
        exit(1);
    }
    
    sem_receiver = sem_open(SEM_RECEIVER_NAME, 0);
    if (sem_receiver == SEM_FAILED) {
        perror(MSG_SEM_OPEN_ERROR);
        printf("%s\n", MSG_IPC_FAILED);
        exit(1);
    }
    
    shm_fd = shm_open(SHM_NAME, O_RDONLY, 0644);
    if (shm_fd == -1) {
        perror(MSG_SHM_CREATE_ERROR);
        printf("%s\n", MSG_IPC_FAILED);
        cleanup();
        exit(1);
    }
    
    shared_data = mmap(NULL, sizeof(shared_data_t), 
                      PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror(MSG_SHM_ATTACH_ERROR);
        cleanup();
        exit(1);
    }
    
    printf("Получатель PID: %d запущен\n", getpid());
    printf("%s\n", MSG_PRESS_CTRL_C);
    
    while (1) {
        printf("%s\n", MSG_WAITING_DATA);
        
        if (sem_wait(sem_receiver) == -1) {
            perror(MSG_SEM_WAIT_ERROR);
            cleanup();
            exit(1);
        }
        
        time_t rawtime;
        struct tm *timeinfo;
        char receiver_time[64];
        
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(receiver_time, sizeof(receiver_time),
                "%Y-%m-%d %H:%M:%S", timeinfo);
        
        printf("%s\n", MSG_READING_DATA);
        printf(MSG_RECEIVER_TIME, receiver_time, getpid());
        printf("\n");
        printf("Получено: %s (PID отправителя: %d)\n", 
               shared_data->time_str, shared_data->pid);
        printf("----------------------------------------\n");
        
        if (sem_post(sem_sender) == -1) {
            perror(MSG_SEM_POST_ERROR);
            cleanup();
            exit(1);
        }
    }
    
    return 0;
}