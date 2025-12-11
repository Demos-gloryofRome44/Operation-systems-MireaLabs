#ifndef MESSAGES_H
#define MESSAGES_H

#define MSG_SHM_CREATE_ERROR "Ошибка создания разделяемой памяти"
#define MSG_SHM_ATTACH_ERROR "Ошибка подключения к разделяемой памяти"
#define MSG_SHM_DETACH_ERROR "Ошибка отключения от разделяемой памяти"
#define MSG_SHM_DELETE_ERROR "Ошибка удаления разделяемой памяти"
#define MSG_SEM_CREATE_ERROR "Ошибка создания семафора"
#define MSG_SEM_OPEN_ERROR "Ошибка открытия семафора"
#define MSG_SEM_WAIT_ERROR "Ошибка ожидания семафора"
#define MSG_SEM_POST_ERROR "Ошибка освобождения семафора"
#define MSG_SEM_CLOSE_ERROR "Ошибка закрытия семафора"
#define MSG_SEM_UNLINK_ERROR "Ошибка удаления семафора"
#define MSG_ALREADY_RUNNING "Программа уже запущена. Завершение."
#define MSG_IPC_FAILED "Ошибка IPC. Завершение."
#define MSG_SHM_EMPTY "Разделяемая память пуста"
#define MSG_READING_DATA "Чтение данных..."
#define MSG_WRITING_DATA "Запись данных..."
#define MSG_READ_FAILED "Ошибка чтения данных"
#define MSG_WRITE_FAILED "Ошибка записи данных"
#define MSG_TIME_FORMAT "Время: %s PID: %d"
#define MSG_SENDER_TIME "Отправитель - %s (PID: %d)"
#define MSG_RECEIVER_TIME "Получатель - %s (PID: %d)"
#define MSG_PRESS_CTRL_C "Нажмите Ctrl+C для завершения"
#define MSG_CLEANUP "Очистка ресурсов..."
#define MSG_WAITING_DATA "Ожидание данных..."

#define SHM_NAME "/time_shm_practice7"
#define SEM_SENDER_NAME "/sem_sender_practice7"
#define SEM_RECEIVER_NAME "/sem_receiver_practice7"
#define SHM_SIZE 256

#endif