#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_STRING_LENGTH 1000
#define LOG_FILE "virus.log"

void replaceStrings(char *content) {
    char *patterns[] = {"m4LwAr3", "5pYw4R3", "R4nS0mWaR3"};
    char *replacements[] = {"[MALWARE]", "[SPYWARE]", "[RANSOMWARE]"};
    int num_patterns = sizeof(patterns) / sizeof(patterns[0]);

    for (int i = 0; i < num_patterns; i++) {
        char *found = strstr(content, patterns[i]);
        while (found != NULL) {
            strncpy(found, replacements[i], strlen(replacements[i]));
            found = strstr(found + strlen(replacements[i]), patterns[i]);
        }
    }
}

void logReplacements(const char *filePath, const char *originalString, const char *replacedString) {
    time_t now;
    struct tm *localTime;
    char timestamp[20];

    time(&now);
    localTime = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d-%m-%Y %H:%M:%S", localTime);

    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile != NULL) {
        fprintf(logFile, "[%s] Suspicious string at %s successfully replaced!\n", timestamp, filePath);
        
        fputs("\n", logFile);
        fclose(logFile);
    }
}

bool processFile(const char *filePath) {
    FILE *file = fopen(filePath, "r+");
    if (file == NULL) {
        printf("Error opening file: %s\n", filePath);
        return false;
    }

    char content[MAX_STRING_LENGTH];
    fread(content, sizeof(char), sizeof(content), file);

    char originalContent[MAX_STRING_LENGTH];
    strcpy(originalContent, content);

    bool stringReplaced = false;

    char *patterns[] = {"m4LwAr3", "5pYw4R3", "R4nS0mWaR3"};
    char *replacements[] = {"[MALWARE]", "[SPYWARE]", "[RANSOMWARE]"};
    int num_patterns = sizeof(patterns) / sizeof(patterns[0]);

    for (int i = 0; i < num_patterns; i++) {
        char *found = strstr(content, patterns[i]);
        while (found != NULL) {
            stringReplaced = true;

            char replacedContent[MAX_STRING_LENGTH];
            strcpy(replacedContent, content);
            strncpy(found, replacements[i], strlen(replacements[i]));

            logReplacements(filePath, patterns[i], replacedContent);

            found = strstr(found + strlen(replacements[i]), patterns[i]);
        }
    }

    if (stringReplaced) {
        fseek(file, 0, SEEK_SET);
        fwrite(content, sizeof(char), strlen(content), file);
    }

    fclose(file);
    return stringReplaced;
}

void processDirectory(const char *dirPath) {
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        printf("Error opening directory: %s\n", dirPath);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        char filePath[MAX_STRING_LENGTH];
        snprintf(filePath, sizeof(filePath), "%s/%s", dirPath, entry->d_name);
        struct stat fileStat;
        if (stat(filePath, &fileStat) == 0) {
            if (S_ISDIR(fileStat.st_mode)) {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                    processDirectory(filePath);
                }
            } else {
                processFile(filePath);
            }
        }
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <folder_path>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        return 1;
    } else if (pid > 0) {
        printf("Daemon started with PID: %d\n", pid);
        return 0;
    }

    while (1) {
        processDirectory(argv[1]);
        sleep(15);
    }

    return 0;
}
