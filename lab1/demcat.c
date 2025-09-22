#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 4096

void handle_text_line(char *text_buffer, int *counter, int display_numbers, 
                     int skip_blanks, int mark_ends)
{
    size_t text_length = strlen(text_buffer);
    int blank_line = (text_length == 1 && text_buffer[0] == '\n');

    if (skip_blanks && blank_line)
    {
        return;
    }

    if (display_numbers)
    {
        printf("%6d\t", (*counter)++);
    }

    if (mark_ends && text_length > 0 && text_buffer[text_length - 1] == '\n')
    {
        text_buffer[text_length - 1] = '\0';
        printf("%s$\n", text_buffer);
    }
    else if (mark_ends)
    {
        printf("%s$", text_buffer);
    }
    else
    {
        fputs(text_buffer, stdout);
    }
}

int main(int argc, char *argv[])  // ← ИЗМЕНИЛ НАЗВАНИЕ ФУНКЦИИ НА main
{
    int number_display = 0;
    int blank_skip = 0;
    int end_marker = 0;

    int current_index = 1;
    while (current_index < argc && argv[current_index][0] == '-')
    {
        char *option_string = argv[current_index];
        for (int pos = 1; option_string[pos] != '\0'; pos++)
        {
            switch (option_string[pos])
            {
            case 'n':
                number_display = 1;
                break;
            case 'b':
                blank_skip = 1;
                number_display = 1;
                break;
            case 'E':
                end_marker = 1;
                break;
            default:
                break;
            }
        }
        current_index++;
    }

    if (current_index >= argc)
    {
        if (isatty(STDIN_FILENO))
        {
            fprintf(stderr, "Reading from standard input... (Use Ctrl+D to finish)\n");
        }

        char input_buffer[MAX_BUFFER_SIZE];
        int line_counter = 1;

        while (fgets(input_buffer, MAX_BUFFER_SIZE, stdin))
        {
            handle_text_line(input_buffer, &line_counter, number_display, blank_skip, end_marker);
        }
    }
    else
    {
        for (; current_index < argc; current_index++)
        {
            FILE *file_pointer = fopen(argv[current_index], "r");
            if (!file_pointer)
            {
                fprintf(stderr, "Unable to access file: %s\n", argv[current_index]);
                continue;
            }

            char file_buffer[MAX_BUFFER_SIZE];
            int count_lines = 1;

            while (fgets(file_buffer, MAX_BUFFER_SIZE, file_pointer))
            {
                handle_text_line(file_buffer, &count_lines, number_display, blank_skip, end_marker);
            }

            fclose(file_pointer);
        }
    }

    return 0;
}