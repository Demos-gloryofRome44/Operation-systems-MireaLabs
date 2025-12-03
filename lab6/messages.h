#ifndef MESSAGES_H
#define MESSAGES_H

#define FIFO_NAME "/tmp/myfifo"
#define BUFFER_SIZE 256
#define PIPE_SLEEP_TIME 5
#define FIFO_SLEEP_TIME 10

#define PIPE_PARENT_SENDING "Родитель (PID: %d) отправил данные и ожидает...\n"
#define PIPE_CHILD_HEADER "=== Дочерний процесс (PID: %d) ==="
#define PIPE_ERROR_CREATE_PIPE "Ошибка создания pipe"
#define PIPE_ERROR_CREATE_PROCESS "Ошибка создания процесса"

#define SENDER_STARTED "Отправитель (PID: %d) начал работу\n"
#define SENDER_COMPLETED "Отправитель завершил передачу данных...\n"
#define RECEIVER_STARTED "Получатель (PID: %d) запущен и ожидает данные...\n"
#define RECEIVER_HEADER "=== Получатель (PID: %d) ==="
#define TIME_DIFFERENCE "Задержка между отправкой и получением: %d секунд\n"

#define MESSAGE_FORMAT "%s (PID: %d)\nВремя отправки: %s"
#define TIME_FORMAT "Локальное время: %s\n"
#define PROCESS_INFO_HEADER "=== Процесс (PID: %d) ==="
#define PROCESS_MESSAGE "Сообщение: %s\n"

#define ERROR_MESSAGE "Не удалось прочитать данные из FIFO"

void get_current_time_str(char* buffer, int size) {
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", time_info);
}

void format_message(char* dest, const char* source, pid_t pid, const char* time_str) {
    snprintf(dest, BUFFER_SIZE, MESSAGE_FORMAT, source, pid, time_str);
}
void print_process_info(pid_t pid, const char* message, const char* time_str) {
    printf("\n");
    printf(PROCESS_INFO_HEADER, pid);
    printf("\n");
    printf(PROCESS_MESSAGE, message);
    printf(TIME_FORMAT, time_str);
}

#endif