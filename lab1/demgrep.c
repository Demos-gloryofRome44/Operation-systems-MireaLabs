#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_SIZE 4096

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s search_pattern [input_files...]\n", argv[0]);
        fprintf(stderr, "Examples:\n");
        fprintf(stderr, "  %s \"hello\" file.txt\n", argv[0]);
        fprintf(stderr, "  cat file.txt | %s \"pattern\"\n", argv[0]);
        fprintf(stderr, "  %s \"error\" *.log\n", argv[0]);
        return 1;
    }

    char *search_pattern = argv[1];
    int total_files = argc - 2;

    printf("🔍 Searching for: \"%s\"\n", search_pattern);
    printf("📁 Files to search: %d\n\n", total_files);

    if (total_files == 0)
    {
        printf("Reading from standard input:\n");
        printf("---------------------------\n");
        
        char input_line[MAX_INPUT_SIZE];
        int match_count = 0;

        while (fgets(input_line, MAX_INPUT_SIZE, stdin))
        {
            if (strstr(input_line, search_pattern) != NULL)
            {
                match_count++;
                printf("Match %d: %s", match_count, input_line);
            }
        }
        
        printf("---------------------------\n");
        printf("Total matches found: %d\n", match_count);
    }
    else
    {
        int total_matches = 0;
        
        for (int i = 2; i < argc; i++)
        {
            FILE *input_file = fopen(argv[i], "r");
            if (!input_file)
            {
                fprintf(stderr, "Error: Could not open file '%s'\n", argv[i]);
                continue;
            }

            printf("Searching in: %s\n", argv[i]);
            printf("----------------------------\n");
            
            char file_line[MAX_INPUT_SIZE];
            int file_match_count = 0;
            int line_number = 1;

            while (fgets(file_line, MAX_INPUT_SIZE, input_file))
            {
                if (strstr(file_line, search_pattern) != NULL)
                {
                    file_match_count++;
                    total_matches++;
                    if (total_files > 1)
                    {
                        printf("%s:%d: %s", argv[i], line_number, file_line);
                    }
                    else
                    {
                        printf("Line %d: %s", line_number, file_line);
                    }
                }
                line_number++;
            }

            printf("Matches in %s: %d\n\n", argv[i], file_match_count);
            fclose(input_file);
        }
        
        printf("====================================\n");
        printf("🎯 Total matches across all files: %d\n", total_matches);
    }

    return 0;
}