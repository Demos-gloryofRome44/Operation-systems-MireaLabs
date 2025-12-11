#ifndef MESSAGES_H
#define MESSAGES_H

#define MSG_WRITER_STARTED "Пишущий поток запущен"
#define MSG_READER_STARTED "Читающий поток %ld запущен"
#define MSG_WRITER_WRITING "Пишущий поток записывает..."
#define MSG_WRITER_WROTE "Пишущий поток: записал '%s'"
#define MSG_READER_READ "Читающий поток %ld: прочитал '%s'"
#define MSG_PROGRAM_STARTED "Программа запущена"
#define MSG_PROGRAM_ENDING "Завершение работы программы..."
#define MSG_PROGRAM_FINISHED "Программа завершена"
#define MSG_THREAD_CANCELLED "Поток %ld отменен"
#define MSG_THREAD_JOINED "Ожидание завершения потоков..."

#define MSG_MUTEX_INIT_ERROR "Ошибка инициализации мьютекса"
#define MSG_MUTEX_DESTROY_ERROR "Ошибка удаления мьютекса"
#define MSG_THREAD_CREATE_ERROR "Ошибка создания потока"
#define MSG_THREAD_JOIN_ERROR "Ошибка ожидания завершения потока"
#define MSG_THREAD_CANCEL_ERROR "Ошибка отмены потока"

#define ARRAY_SIZE 100
#define NUM_READERS 10
#define PROGRAM_DURATION 10
#define WRITER_SLEEP_TIME 2
#define READER_SLEEP_TIME 1

#endif