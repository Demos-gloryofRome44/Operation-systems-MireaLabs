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
        if (shmdt(shared_memory_ptr) < 0) {
            fprintf(stderr, SIGNAL_DETACH_ERROR, strerror(errno));
        }
    }

    if (shmctl(shm_identifier, IPC_RMID, NULL) < 0) {
        fprintf(stderr, SIGNAL_REMOVE_ERROR, strerror(errno));
    }

    if (semctl(sem_identifier, 0, IPC_RMID) == -1) {
        fprintf(stderr, SIGNAL_SEM_REMOVE_ERROR, strerror(errno));
    }

    remove(SHARED_KEY_FILE_NAME);
    exit(0);
}

void setup_signal_handlers() {
    if (signal(SIGINT, cleanup_on_exit) == SIG_ERR ||
        signal(SIGTERM, cleanup_on_exit) == SIG_ERR) {
        handle_error("signal", strerror(errno));
    }
}

key_t generate_ipc_key() {
    int fd = open(SHARED_KEY_FILE_NAME, O_CREAT | O_EXCL, S_IWUSR | S_IRUSR);
    if (fd == -1) {
        fprintf(stderr, "%s\n", ERROR_DUPLICATE_INSTANCE);
        exit(EXIT_FAILURE);
    }
    close(fd);

    key_t key = ftok(SHARED_KEY_FILE_NAME, 'A');
    if (key == -1) {
        handle_error("ftok", strerror(errno));
    }
    return key;
}

void init_shared_memory(key_t key) {
    shm_identifier = shmget(key, BUFFER_CAPACITY, 0666 | IPC_CREAT | IPC_EXCL);
    if (shm_identifier == -1) {
        handle_error("shmget", strerror(errno));
    }

    shared_memory_ptr = (char*)shmat(shm_identifier, NULL, 0);
    if (shared_memory_ptr == (char*)-1) {
        handle_error("shmat", strerror(errno));
    }
}

void init_semaphore(key_t key) {
    sem_identifier = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if (sem_identifier == -1) {
        handle_error("semget", strerror(errno));
    }

    if (semctl(sem_identifier, 0, SETVAL, 1) == -1) {
        handle_error("semctl", strerror(errno));
    }
}

void prepare_message(char* buffer, int msg_num) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    snprintf(buffer, MAX_MESSAGE_LENGTH,
             IPC_SENDER_MESSAGE_FORMAT,
             msg_num,
             tm_info->tm_hour,
             tm_info->tm_min,
             tm_info->tm_sec,
             getpid());
}

void sender_loop() {
    int message_count = 0;
    
    while (1) {
        semop(sem_identifier, &lock_op, 1);

        char message[MAX_MESSAGE_LENGTH];
        prepare_message(message, ++message_count);
        
        strncpy(shared_memory_ptr, message, BUFFER_CAPACITY - 1);
        shared_memory_ptr[BUFFER_CAPACITY - 1] = '\0';
        
        printf("Sender: %s\n", message);
        
        semop(sem_identifier, &unlock_op, 1);
        sleep(3);
    }
}

int main() {
    setup_signal_handlers();
    
    key_t ipc_key = generate_ipc_key();
    init_shared_memory(ipc_key);
    init_semaphore(ipc_key);
    
    printf(IPC_SENDER_STARTED, getpid());
    printf("\n");
    
    sender_loop();
    
    return 0;
}