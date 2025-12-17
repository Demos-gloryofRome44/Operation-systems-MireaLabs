#ifndef MESSAGES_H
#define MESSAGES_H

#define SHARED_KEY_FILE_NAME "ipc_shared_key"
#define BUFFER_CAPACITY 1024
#define TIME_FORMAT_BUFFER 30
#define MAX_MESSAGE_LENGTH 100

#define THREADS_INITIAL_BUFFER_CONTENT "Initial buffer state"
#define THREADS_WRITER_START "Writer thread started (TID: %lu)"
#define THREADS_READER_START "Reader thread started (TID: %lu)"
#define THREADS_WRITER_LOG "Writer (TID: %lu): Wrote - %s"
#define THREADS_READER_LOG "Reader (TID: %lu) | Time: %s | Buffer: %s"
#define THREADS_MAIN_START "Main program started. Press Ctrl+C to exit."

#define IPC_SENDER_STARTED "Sender process initialized (PID: %d)"
#define IPC_RECEIVER_STARTED "Receiver process initialized (PID: %d)"
#define IPC_SENDER_MESSAGE_FORMAT "[Message #%d] Time: %02d:%02d:%02d, PID: %d"
#define IPC_RECEIVER_FORMAT "[Receiver] Local time: %s, PID: %d, Data: %s"

#define ERROR_TEMPLATE "Error in %s: %s"
#define ERROR_SEMAPHORE_INIT "Failed to initialize semaphore"
#define ERROR_THREAD_CREATE "Failed to create thread"
#define ERROR_SEMAPHORE_OPS "Semaphore operation failed"
#define ERROR_SHARED_MEMORY "Shared memory error"
#define ERROR_SEMAPHORE_CREATE "Semaphore creation failed"
#define ERROR_MEMORY_ATTACH "Memory attachment failed"
#define ERROR_SIGNAL_SETUP "Signal handler setup failed"
#define ERROR_KEY_GENERATION "IPC key generation failed"
#define ERROR_DUPLICATE_INSTANCE "Another instance is running"

#define SIGNAL_HANDLER_LOG "[Signal Handler] Received signal: %d"
#define SIGNAL_DETACH_ERROR "Failed to detach shared memory: %s"
#define SIGNAL_REMOVE_ERROR "Failed to remove shared memory: %s"
#define SIGNAL_SEM_REMOVE_ERROR "Failed to remove semaphore: %s"

#endif