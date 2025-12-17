#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include "messages.h"

int shm_identifier = 0;
int sem_identifier = 0;
char* shared_memory_ptr = NULL;

struct sembuf lock_op = {0, -1, 0};
struct sembuf unlock_op = {0, 1, 0};

void handle_error(const char* function, const char* message) {
    fprintf(stderr, ERROR_TEMPLATE, function, message);
    exit(EXIT_FAILURE);
}

void cleanup_on_exit(int sig_num) {
    printf(SIGNAL_HANDLER_LOG, sig_num);

    if (shared_memory_ptr != NULL) {
        if (shmdt(shared_memory_ptr) == -1) {
            fprintf(stderr, SIGNAL_DETACH_ERROR, strerror(errno));
        }
    }
    exit(0);
}

void setup_signal_handlers() {
    if (signal(SIGINT, cleanup_on_exit) == SIG_ERR ||
        signal(SIGTERM, cleanup_on_exit) == SIG_ERR) {
        handle_error("signal", strerror(errno));
    }
}

key_t get_ipc_key() {
    key_t key = ftok(SHARED_KEY_FILE_NAME, 'A');
    if (key == (key_t)-1) {
        handle_error("ftok", strerror(errno));
    }
    return key;
}

void connect_shared_memory(key_t key) {
    shm_identifier = shmget(key, BUFFER_CAPACITY, 0666);
    if (shm_identifier < 0) {
        handle_error("shmget", strerror(errno));
    }

    shared_memory_ptr = shmat(shm_identifier, NULL, 0);
    if (shared_memory_ptr == (void*)-1) {
        handle_error("shmat", strerror(errno));
    }
}

void connect_semaphore(key_t key) {
    sem_identifier = semget(key, 1, 0666);
    if (sem_identifier == -1) {
        handle_error("semget", strerror(errno));
    }
}

void get_current_time_string(char* buffer) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(buffer, TIME_FORMAT_BUFFER, "%H:%M:%S", tm_info);
}

void receiver_loop() {
    while (1) {
        semop(sem_identifier, &lock_op, 1);

        char received_data[MAX_MESSAGE_LENGTH];
        strncpy(received_data, shared_memory_ptr, sizeof(received_data) - 1);
        received_data[sizeof(received_data) - 1] = '\0';

        char time_string[TIME_FORMAT_BUFFER];
        get_current_time_string(time_string);
        
        printf(IPC_RECEIVER_FORMAT, time_string, getpid(), received_data);
        printf("\n");
        
        semop(sem_identifier, &unlock_op, 1);
        sleep(1);
    }
}

int main() {
    setup_signal_handlers();
    
    key_t ipc_key = get_ipc_key();
    connect_shared_memory(ipc_key);
    connect_semaphore(ipc_key);
    
    printf(IPC_RECEIVER_STARTED, getpid());
    printf("\n");
    
    receiver_loop();
    
    return 0;
}