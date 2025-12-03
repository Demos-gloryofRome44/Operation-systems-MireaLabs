#ifndef MESSAGES_H
#define MESSAGES_H

#define MAX_NAME_LENGTH 256
#define BUFFER_SIZE 4096

#define HELP_TEXT \
"Использование:\n" \
"  1) ./archiver arch_name –i(--input) file1     Добавить файл в архив\n" \
"  2) ./archiver arch_name –e(--extract) file1   Извлечь файл из архива (и удалить из архива)\n" \
"  3) ./archiver arch_name –s(--stat)  вывести текущее состояние архива. \n" \
"  4) ./archiver –h(--help)  вывести справку по работе с архиватором  \n"      

#define ERROR_OPEN_ARCHIVE "Ошибка открытия архива"
#define ERROR_OPEN_FILE "Ошибка открытия файла"
#define ERROR_CREATE_FILE "Ошибка создания файла"
#define ERROR_READ_FILE "Ошибка чтения файла"
#define ERROR_WRITE_FILE "Ошибка записи в файл"
#define ERROR_STAT_FILE "Ошибка получения информации о файле"
#define ERROR_FILE_NOT_FOUND "Файл не найден в архиве"
#define ERROR_CREATE_TEMP "Ошибка создания временного файла"
#define ERROR_RENAME_TEMP "Ошибка переименования временного файла"
#define ERROR_NO_ARGUMENTS "Недостаточно аргументов"
#define ERROR_UNKNOWN_COMMAND "Неизвестная команда"

#define SUCCESS_ADDED "Файл добавлен в архив"
#define SUCCESS_EXTRACTED "Файл извлечен"
#define SUCCESS_REMOVED "Файл удален из архива"

#define ARCHIVE_HEADER "Содержимое архива '%s':"
#define SEPARATOR "----------------------------------------"
#define FILE_FORMAT "%-30s %10ld байт"
#define ARCHIVE_EMPTY "Архив пуст"
#define TOTAL_FILES "Всего файлов: %d"

#endif // MESSAGES_H