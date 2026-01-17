#ifndef MESSAGES_H
#define MESSAGES_H

#define BUFFER_SIZE 100
#define READER_COUNT 10
#define WRITER_COUNT 1
#define MAX_OPERATIONS 20

#define MSG_READERS_COUNT "Количество читающих потоков: %d\n"
#define MSG_WRITERS_COUNT "Количество пишущих потоков: %d\n"
#define MSG_MAX_OPERATIONS "Максимальное количество операций записи: %d\n\n"

#define MSG_READER_FORMAT "Читатель %d (TID: %p): Буфер = %s\n"
#define MSG_WRITER_WRITE "\nПисатель (TID: %p): Записал '%s'\n\n"
#define MSG_READER_FINISHED "Читатель %d завершил работу\n"
#define MSG_WRITER_FINISHED "Писатель завершил работу после %d операций\n"

#define MSG_INIT_BUFFER "Начальное содержимое буфера"
#define MSG_WRITE_FORMAT "Запись #%d от писателя (TID: %p)"

#define MSG_RWLOCK_INIT_ERROR "Ошибка инициализации rwlock: %d\n"
#define MSG_READER_CREATE_ERROR "Ошибка создания потока читателя %d: %d\n"
#define MSG_WRITER_CREATE_ERROR "Ошибка создания потока писателя: %d\n"

#define MSG_PROGRAM_FINISHED "\n=== Программа завершена ===\n"
#define MSG_TOTAL_WRITES "Всего операций записи: %d\n"

#endif 