#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <regex.h>
#include <sys/stat.h>

int matches(char* string, char* pattern);

void printPermissions(struct stat fileStat);

int main(int argc, char* argv[]) {
    int ptype;
    char* ep;
    struct stat fileStat;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <permissions> <file>\n", argv[0]);
        exit(1);
    }

    ptype = 1;
    if (!(matches(argv[1], "[0-7]{4}"))) {
        ptype = 0;
        if (!(matches(argv[1], "^[ug]+[+-]+s$"))) {
            fprintf(stderr, "usage: %s <permissions> <file>\n", argv[0]);
            exit(2);
        }
    }

    if (ptype) {
        if ((chmod(argv[2], strtol(argv[1], &ep, 8))) == -1) {
            perror(argv[2]);
            exit(3);
        }
    } else {
        if ((stat(argv[2], &fileStat)) == -1) {
            perror(argv[2]);
            exit(4);
        }

        if (argv[1][0] == 'u') {
            if (argv[1][1] == '+') {
                if ((chmod(argv[2], fileStat.st_mode | S_ISUID)) == -1) {
                    perror(argv[2]);
                    exit(5);
                }
            } else {
                if ((chmod(argv[2], fileStat.st_mode & ~S_ISUID)) == -1) {
                    perror(argv[2]);
                    exit(5);
                }
            }
        } else {
            if (argv[1][1] == '+') {
                if ((chmod(argv[2], fileStat.st_mode | S_ISGID)) == -1) {
                    perror(argv[2]);
                    exit(6);
                }
            } else {
                if ((chmod(argv[2], fileStat.st_mode & ~S_ISGID)) == -1) {
                    perror(argv[2]);
                    exit(6);
                }
            }
        }
    }

    if ((stat(argv[2], &fileStat)) == -1) {
        perror(argv[2]);
        exit(7);
    }

    printPermissions(fileStat);
    
    return 0;
}

int matches(char* string, char* pattern) {
    regex_t regex;
    int status;

    if ((regcomp(&regex, pattern, REG_EXTENDED|REG_NOSUB)) != 0) {
        perror(pattern);
        exit(10);
    }
    
    status = regexec(&regex, string, 0, NULL, 0);
    
    regfree(&regex);

    return (status == 0);
}

void printPermissions(struct stat fileStat) {
    printf("Nova dovoljenja: ");
    printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    if ((fileStat.st_mode & S_ISUID)) {
        printf("s");
    } else {
        printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    }
    printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    if ((fileStat.st_mode & S_ISGID)) {
        printf("s");
    } else {
        printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    }
    printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    if ((fileStat.st_mode & S_ISVTX)) {
        printf("t");
    } else {
        printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");
    }
    printf("\n");
}