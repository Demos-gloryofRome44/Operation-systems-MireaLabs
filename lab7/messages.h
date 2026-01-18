#ifndef MESSAGES_H
#define MESSAGES_H

#define SHM_KEY_FILE "shm_ipc.key"
#define SHM_KEY_ID 65
#define SHM_SIZE 512
#define MAX_OPERATIONS 50

#define MSG_SENDER_START "Отправитель (PID: %d) запущен.\n"
#define MSG_ALREADY_RUNNING "Программа отправителя уже запущена.\n"
#define MSG_SHARED_MEM_CREATED "Разделяемая память создана успешно.\n"
#define MSG_WRITING_DATA ">>> Запись данных в разделяемую память...\n"
#define MSG_DATA_WRITTEN "Время: %s | PID отправителя: %d\n"
#define MSG_SENDER_EXIT "Отправитель завершает работу...\n"
#define MSG_CLEANUP_RESOURCES "Освобождение ресурсов...\n"
#define MSG_OPERATIONS_COMPLETED "Выполнено операций записи: %d\n"
#define MSG_CTRL_C_HINT "Нажмите Ctrl+C для завершения работы.\n"

#define MSG_RECEIVER_START "Получатель (PID: %d) запущен и подключился к разделяемой памяти.\n"
#define MSG_IPC_SETUP_FAILED "Ошибка настройки межпроцессного взаимодействия.\n"
#define MSG_SHARED_MEM_CONNECTED "Успешное подключение к разделяемой памяти.\n"
#define MSG_READING_DATA ">>> Чтение данных из разделяемой памяти...\n"
#define MSG_DATA_READ "Локальное время: %s | PID получателя: %d\n"
#define MSG_RECEIVED_DATA "Полученные данные: %s\n"
#define MSG_SEPARATOR "----------------------------------------\n"
#define MSG_RECEIVER_EXIT "Получатель завершает работу...\n"
#define MSG_WAITING_NEW_DATA "Ожидание новых данных...\n"

#define TIME_FORMAT_LONG "%Y-%m-%d %H:%M:%S"
#define TIME_FORMAT_SHORT "%H:%M:%S"

#endif // MESSAGES_H