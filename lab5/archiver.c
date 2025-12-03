#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <errno.h>

#include "messages.h"

struct archive_entry {
    char name[MAX_NAME_LENGTH];
    off_t size;
    mode_t mode;
    uid_t uid;
    gid_t gid;
    time_t atime;
    time_t mtime;
};

void show_help(void);
int show_contents(const char *archive_name);
int add_to_archive(const char *archive_name, const char *filename);
int extract_from_archive(const char *archive_name, const char *filename);

int read_entry(int fd, struct archive_entry *entry);
int write_entry(int fd, struct archive_entry *entry);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf(HELP_TEXT);
        return 1;
    }
    
    if (strcmp(argv[1], "-h") == 0) {
        printf(HELP_TEXT);
        return 0;
    }
    
    if (argc < 3) {
        printf(HELP_TEXT);
        return 1;
    }
    
    const char *archive_name = argv[1];
    const char *option = argv[2];
    
    if (strcmp(option, "-i") == 0 && argc >= 4) {
        return add_to_archive(archive_name, argv[3]);
    } else if (strcmp(option, "-e") == 0 && argc >= 4) {
        return extract_from_archive(archive_name, argv[3]);
    } else if (strcmp(option, "-s") == 0) {
        return show_contents(archive_name);
    } else {
        printf(HELP_TEXT);
        return 1;
    }
}

void show_help(void) {
    printf(HELP_TEXT);
}

int read_entry(int fd, struct archive_entry *entry) {
    if (read(fd, entry, sizeof(struct archive_entry)) != sizeof(struct archive_entry)) {
        return -1;
    }
    return 0;
}

int write_entry(int fd, struct archive_entry *entry) {
    if (write(fd, entry, sizeof(struct archive_entry)) != sizeof(struct archive_entry)) {
        return -1;
    }
    return 0;
}

int show_contents(const char *archive_name) {
    int archive_fd = open(archive_name, O_RDONLY);
    if (archive_fd < 0) {
        perror(ERROR_OPEN_ARCHIVE);
        return 1;
    }
    
    printf(ARCHIVE_HEADER "\n", archive_name);
    printf(SEPARATOR "\n");
    
    struct archive_entry entry;
    int count = 0;
    
    while (read_entry(archive_fd, &entry) == 0) {
        printf(FILE_FORMAT "\n", entry.name, (long)entry.size);
        lseek(archive_fd, entry.size, SEEK_CUR);
        count++;
    }
    
    if (count == 0) {
        printf(ARCHIVE_EMPTY "\n");
    }
    
    printf(SEPARATOR "\n");
    printf(TOTAL_FILES "\n", count);
    
    close(archive_fd);
    return 0;
}

int add_to_archive(const char *archive_name, const char *filename) {
    struct stat file_info;
    if (stat(filename, &file_info) < 0) {
        perror(ERROR_STAT_FILE);
        return 1;
    }

    int archive_fd = open(archive_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (archive_fd < 0) {
        perror(ERROR_OPEN_ARCHIVE);
        return 1;
    }

    struct archive_entry entry;
    strncpy(entry.name, filename, MAX_NAME_LENGTH - 1);
    entry.name[MAX_NAME_LENGTH - 1] = '\0';
    entry.size = file_info.st_size;
    entry.mode = file_info.st_mode;
    entry.uid = file_info.st_uid;
    entry.gid = file_info.st_gid;
    entry.atime = file_info.st_atime;
    entry.mtime = file_info.st_mtime;

    if (write_entry(archive_fd, &entry) < 0) {
        perror(ERROR_WRITE_FILE);
        close(archive_fd);
        return 1;
    }

    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror(ERROR_OPEN_FILE);
        close(archive_fd);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, BUFFER_SIZE)) > 0) {
        if (write(archive_fd, buffer, bytes_read) != bytes_read) {
            perror(ERROR_WRITE_FILE);
            close(file_fd);
            close(archive_fd);
            return 1;
        }
    }

    if (bytes_read < 0) {
        perror(ERROR_READ_FILE);
        close(file_fd);
        close(archive_fd);
        return 1;
    }

    close(file_fd);
    close(archive_fd);
    
    printf("%s: '%s'\n", SUCCESS_ADDED, filename);
    return 0;
}

int extract_from_archive(const char *archive_name, const char *filename) {
    int archive_fd = open(archive_name, O_RDWR);
    if (archive_fd < 0) {
        perror(ERROR_OPEN_ARCHIVE);
        return 1;
    }

    int temp_fd = open("temp_archive", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (temp_fd < 0) {
        perror(ERROR_CREATE_TEMP);
        close(archive_fd);
        return 1;
    }

    struct archive_entry entry;
    int found = 0;
    
    lseek(archive_fd, 0, SEEK_SET);
    
    while (read_entry(archive_fd, &entry) == 0) {
        char buffer[BUFFER_SIZE];
        off_t bytes_processed = 0;
        
        if (strcmp(entry.name, filename) == 0) {
            found = 1;
            
            int output_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, entry.mode);
            if (output_fd < 0) {
                perror(ERROR_CREATE_FILE);
                close(archive_fd);
                close(temp_fd);
                return 1;
            }
            
            while (bytes_processed < entry.size) {
                ssize_t to_read = (entry.size - bytes_processed) > BUFFER_SIZE ? 
                                 BUFFER_SIZE : (entry.size - bytes_processed);
                ssize_t rd = read(archive_fd, buffer, to_read);
                if (rd <= 0) break;
                
                if (write(output_fd, buffer, rd) != rd) {
                    perror(ERROR_WRITE_FILE);
                    close(output_fd);
                    close(archive_fd);
                    close(temp_fd);
                    return 1;
                }
                bytes_processed += rd;
            }
            
            close(output_fd);
            
            struct utimbuf times;
            times.actime = entry.atime;
            times.modtime = entry.mtime;
            utime(filename, &times);
            
            chown(filename, entry.uid, entry.gid);
            chmod(filename, entry.mode);
            
            printf("%s: '%s'\n", SUCCESS_EXTRACTED, filename);
            
            continue;
        }
        
        write_entry(temp_fd, &entry);
        
        bytes_processed = 0;
        while (bytes_processed < entry.size) {
            ssize_t to_read = (entry.size - bytes_processed) > BUFFER_SIZE ? 
                             BUFFER_SIZE : (entry.size - bytes_processed);
            ssize_t rd = read(archive_fd, buffer, to_read);
            if (rd <= 0) break;
            
            if (write(temp_fd, buffer, rd) != rd) {
                perror(ERROR_WRITE_FILE);
                close(archive_fd);
                close(temp_fd);
                return 1;
            }
            bytes_processed += rd;
        }
    }
    
    close(archive_fd);
    close(temp_fd);
    
    if (!found) {
        printf("%s: '%s'\n", ERROR_FILE_NOT_FOUND, filename);
        unlink("temp_archive");
        return 1;
    }
    
    if (rename("temp_archive", archive_name) < 0) {
        perror(ERROR_RENAME_TEMP);
        return 1;
    }
    
    printf("%s: '%s'\n", SUCCESS_REMOVED, filename);
    return 0;
}