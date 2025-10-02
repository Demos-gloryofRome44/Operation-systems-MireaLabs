#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>
#include <getopt.h>


enum ListFlags
{
    LS_ALL = 1,
    LS_LONG = 2
};

enum FileColor
{
    COL_FILE,
    COL_DIR,
    COL_EXEC,
    COL_LN
};

enum ErrorCode
{
    ERR_NEARGS,
    ERR_INVALIDOPT,
    ERR_FILE_ISNT_SPEC,
    ERR_OPENDIR,
    ERR_READDIR,
    ERR_STAT,
    ERR_PWD,
    ERR_GRP
};

static const int FILE_TYPE_COLORS[] = {
    39,  // COL_FILE - обычный файл (белый/стандартный)
    34,  // COL_DIR - директория (синий)
    32,  // COL_EXEC - исполняемый файл (зеленый)
    35   // COL_LN - символическая ссылка (пурпурный)
};

static const char * const VALID_OPTIONS = "hla";
static const char * const PERMISSION_CHARS = "rwx";
static const int TIME_STRING_OFFSET = 4;
static const int TIME_STRING_LENGTH = 12;
static const char * const ANSI_RESET = "\x1b[0m";
static const char * const ANSI_COLOR_PREFIX = "\x1b[;";
const char * g_basePath = NULL;
static const size_t MAX_PATH_LENGTH =
#ifdef PATH_MAX
    (size_t)PATH_MAX
#else
    (size_t)4096
#endif
;
char * g_pathBuffer = NULL;

DIR *g_openDir = NULL;
size_t g_entryCount = 0;
struct dirent ** g_entries = NULL;
int g_flags = 0;

void initProgram();
void failWithError(enum ErrorCode);
void composePath(const char * fileName);
void collectEntries();
void printEntry(struct dirent* entry);
void listDirectory(const char *dirPath);
void closeDirOnExit();
void freePathOnExit();
void freeEntryListOnExit();
int direntCompare(const void* a, const void* b);


int main(int argc, char **argv){
    initProgram();

    opterr = 0; 
    int option;
    while ((option = getopt(argc, argv, VALID_OPTIONS)) != -1){
        switch (option){
            case 'h':
                printf("ls - list directory contents\n"
                       "usage: ls [params...] [file]\n"
                       " -a - do not ignore entries starting with '.'\n"
                       " -l - use a long listing format\n"
                       " -h - print this message\n");
                exit(EXIT_SUCCESS);
            case 'l':
                g_flags |= LS_LONG;
                break;
            case 'a':
                g_flags |= LS_ALL;
                break;
            default:
                failWithError(ERR_INVALIDOPT);
        }
    }

    const char *targetDir = (optind == argc) ? "." : argv[optind];
    listDirectory(targetDir);
    exit(EXIT_SUCCESS);
}

void initProgram(){
    g_pathBuffer = (char *)malloc(MAX_PATH_LENGTH);
    atexit(freePathOnExit);
}

void failWithError(enum ErrorCode err){
    switch (err){
    case ERR_NEARGS:
        fprintf(stderr, "[ls]: Not enough args! Usage: ls -h\n");
        break;
    case ERR_INVALIDOPT:
        fprintf(stderr, "[ls]: Error: invalid option, see \"ls -h\"\n");
        break;
    case ERR_FILE_ISNT_SPEC:
        fprintf(stderr, "[ls]: Error! File isn't specified\n");
        break;
    case ERR_OPENDIR:
        fprintf(stderr, "[ls]: Error while opening directory! %s\n",
                strerror(errno));
        break;
    case ERR_READDIR:
        fprintf(stderr, "[ls]: Error while parsing files! %s\n",
                strerror(errno));
        break;
    case ERR_STAT:
        fprintf(stderr, "[ls]: error while getting stat! %s\n",
                strerror(errno));
        break;
    case ERR_PWD:
        fprintf(stderr, "[ls]: error while getting username! %s\n",
                strerror(errno));
        break;
    case ERR_GRP:
        fprintf(stderr, "[ls]: error while getting groupname! %s\n",
                strerror(errno));
        break;
    }
    exit(EXIT_FAILURE);
}

void composePath(const char * fileName){
    if (g_basePath == NULL){
        g_basePath = ".";
    }
    int written = snprintf(g_pathBuffer, MAX_PATH_LENGTH, "%s/%s", g_basePath, fileName);
    if (written < 0 || (size_t)written >= MAX_PATH_LENGTH){
        fprintf(stderr, "[ls]: path too long\n");
        exit(EXIT_FAILURE);
    }
}

void collectEntries(){
    errno = 0;
    for (struct dirent *cur_file; (cur_file = readdir(g_openDir)) != NULL;
         g_entryCount++);
    if (errno){
        failWithError(ERR_READDIR);
    }
    g_entries = (struct dirent **)malloc(g_entryCount * sizeof(struct dirent *));
    atexit(freeEntryListOnExit);

    rewinddir(g_openDir);
    int i = 0;
    for (struct dirent *cur_file; (cur_file = readdir(g_openDir)) != NULL;
         g_entries[i++] = cur_file);
    qsort(g_entries, g_entryCount, sizeof(struct dirent *), direntCompare);
}

void printEntry(struct dirent *entry){
    if (entry->d_name[0] == '.' && !(g_flags & LS_ALL)){
        return;
    }
    composePath(entry->d_name);
    enum FileColor filename_color = COL_FILE;
    struct stat file_info;
    if (lstat(g_pathBuffer, &file_info) == -1) {
        failWithError(ERR_STAT);
    }

    if (g_flags & LS_LONG){
        char fileTypeChar = '?';
        if (S_ISREG(file_info.st_mode)){
            fileTypeChar = '-';
            if (file_info.st_mode & S_IXUSR){
                filename_color = COL_EXEC;
            }
        }
        else if (S_ISDIR(file_info.st_mode)){
            fileTypeChar = 'd';
            filename_color = COL_DIR;
        }
        else if (S_ISBLK(file_info.st_mode)){
            fileTypeChar = 'b';
        }
        else if (S_ISLNK(file_info.st_mode)){
            fileTypeChar = 'l';
            filename_color = COL_LN;
        }
        putchar(fileTypeChar);

        int i = 0;
        for (uint64_t mask = S_IRUSR; mask > 0; mask >>= 1, i++){
            putchar(file_info.st_mode & mask ? PERMISSION_CHARS[i%3] : '-');
        }
        putchar(' ');

        errno = 0;
        struct passwd *pwd_file = getpwuid(file_info.st_uid);
        if (pwd_file == NULL && errno){
            failWithError(ERR_PWD);
        }

        errno = 0;
        struct group *grp_file = getgrgid(file_info.st_gid);
        if (grp_file == NULL && errno){
            failWithError(ERR_GRP);
        }

        printf("%hu %s %s %lld ", 
               (unsigned short)file_info.st_nlink,  
               pwd_file->pw_name, 
               grp_file->gr_name, 
               (long long)file_info.st_size);  

        char *time_str = ctime(&file_info.st_mtime);
        char output_time_str[TIME_STRING_LENGTH + 1];
        strncpy(output_time_str, time_str + TIME_STRING_OFFSET, TIME_STRING_LENGTH);
        output_time_str[TIME_STRING_LENGTH] = '\0';
        printf("%s ", output_time_str);

        if (strchr(entry->d_name, ' ') != NULL){
            printf("%s%dm`%s`%s\n", ANSI_COLOR_PREFIX, FILE_TYPE_COLORS[filename_color],
                   entry->d_name, ANSI_RESET);
        }
        else{
            printf("%s%dm%s%s\n", ANSI_COLOR_PREFIX, FILE_TYPE_COLORS[filename_color],
                   entry->d_name, ANSI_RESET);
        }
    } else{
        if (S_ISREG(file_info.st_mode) && (file_info.st_mode & S_IXUSR)){
            filename_color = COL_EXEC;
        }
        else if (S_ISDIR(file_info.st_mode)){
            filename_color = COL_DIR;
        }
        else if (S_ISLNK(file_info.st_mode)){
            filename_color = COL_LN;
        }
        if (strchr(entry->d_name, ' ') != NULL){
            printf("%s%dm`%s`%s  ", ANSI_COLOR_PREFIX, FILE_TYPE_COLORS[filename_color], entry->d_name, ANSI_RESET);
        }
        else{
            printf("%s%dm%s%s  ", ANSI_COLOR_PREFIX, FILE_TYPE_COLORS[filename_color], entry->d_name, ANSI_RESET);
        }
    }
}

void listDirectory(const char *dirPath){
    g_openDir = opendir(dirPath);
    if (g_openDir == NULL){
        failWithError(ERR_OPENDIR);
    }

    atexit(closeDirOnExit);
    g_basePath = dirPath;
    errno = 0; 
    collectEntries();
    for (size_t i = 0; i < g_entryCount; ++i){
        printEntry(g_entries[i]);
    }
    if (!(g_flags & LS_LONG)){
        putchar('\n');
    }
}

void closeDirOnExit() { if (g_openDir) { closedir(g_openDir); g_openDir = NULL; } }
void freePathOnExit() { free(g_pathBuffer); }
void freeEntryListOnExit() { free(g_entries); }

int direntCompare(const void *a, const void *b){
    struct dirent *f = *(struct dirent **)a, *s = *(struct dirent **)b;
    
    int f_dot = (f->d_name[0] == '.');
    int s_dot = (s->d_name[0] == '.');
    
    if (f_dot && !s_dot) return -1;
    if (!f_dot && s_dot) return 1;
    
    return strcasecmp(f->d_name, s->d_name);
}