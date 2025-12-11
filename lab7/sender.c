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
    printf("%s\n", MSG_CLEANUP);
    
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
        sem_unlink(SEM_RECEIVER_NAME);
    }
    
    shm_unlink(SHM_NAME);
    sem_unlink(SEM_SENDER_NAME);
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        cleanup();
        exit(0);
    }
}

int main() {
    signal(SIGINT, signal_handler);
    
    sem_sender = sem_open(SEM_SENDER_NAME, O_CREAT | O_EXCL, 0644, 1);
    if (sem_sender == SEM_FAILED) {
        printf("%s\n", MSG_ALREADY_RUNNING);
        exit(1);
    }
    
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0644);
    if (shm_fd == -1) {
        perror(MSG_SHM_CREATE_ERROR);
        cleanup();
        exit(1);
    }
    
    if (ftruncate(shm_fd, sizeof(shared_data_t)) == -1) {
        perror(MSG_SHM_CREATE_ERROR);
        cleanup();
        exit(1);
    }
    
    shared_data = mmap(NULL, sizeof(shared_data_t), 
                      PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_data == MAP_FAILED) {
        perror(MSG_SHM_ATTACH_ERROR);
        cleanup();
        exit(1);
    }
    
    sem_receiver = sem_open(SEM_RECEIVER_NAME, O_CREAT, 0644, 0);
    if (sem_receiver == SEM_FAILED) {
        perror(MSG_SEM_CREATE_ERROR);
        cleanup();
        exit(1);
    }
    
    printf("%s\n", MSG_PRESS_CTRL_C);
    
    while (1) {
        if (sem_wait(sem_sender) == -1) {
            perror(MSG_SEM_WAIT_ERROR);
            cleanup();
            exit(1);
        }
        
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(shared_data->time_str, sizeof(shared_data->time_str),
                "%Y-%m-%d %H:%M:%S", timeinfo);
        
        shared_data->pid = getpid();
        
        printf("%s\n", MSG_WRITING_DATA);
        printf(MSG_SENDER_TIME, shared_data->time_str, shared_data->pid);
        printf("\n");
        
        if (sem_post(sem_receiver) == -1) {
            perror(MSG_SEM_POST_ERROR);
            cleanup();
            exit(1);
        }
        
        sleep(1); 
    }
    
    return 0;
}