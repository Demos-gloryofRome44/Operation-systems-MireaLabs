#ifndef MESSAGES_H
#define MESSAGES_H

#define ARRAY_SIZE 100
#define NUM_READERS 10
#define PROGRAM_DURATION 10
#define WRITER_SLEEP_TIME 2
#define READER_SLEEP_TIME 1

#define MSG_PROGRAM_START "=== Программа запущена ==="
#define MSG_THREADS_CREATED "Создается %d читающих потоков и 1 пишущий"
#define MSG_PROGRAM_WORKING "Программа будет работать %d секунд..."
#define MSG_PROGRAM_END "=== Завершение работы программы ==="
#define MSG_WAITING_THREADS "Ожидание завершения потоков..."
#define MSG_PROGRAM_FINISH "=== Программа завершена ==="
#define MSG_TOTAL_WRITES "Всего было сделано записей: %d"

#define MSG_WRITER_STARTED "Пишущий поток начал работу"
#define MSG_READER_STARTED "Читающий поток %ld начал работу"
#define MSG_WRITER_WRITING "Пишущий поток записывает данные..."
#define MSG_WRITER_WROTE "Пишущий поток записал: %s"
#define MSG_READER_READ "Читающий поток %ld: TID=0x%lx, содержимое: %s"

#define MSG_MUTEX_INIT_ERROR "Ошибка инициализации мьютекса"
#define MSG_MUTEX_DESTROY_ERROR "Ошибка уничтожения мьютекса"
#define MSG_THREAD_CREATE_ERROR "Ошибка создания потока"
#define MSG_THREAD_JOIN_ERROR "Ошибка ожидания завершения потока"
#define MSG_THREAD_CANCEL_ERROR "Ошибка отмены потока"

#endif