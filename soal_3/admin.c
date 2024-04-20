#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <limits.h>
#include <pwd.h>

void logProcess(char *user, int pid, char *name, int status) {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];
    FILE *logFile;
    char logFileName[100];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d:%m:%Y-%H:%M:%S", timeinfo);
    sprintf(logFileName, "%s.log", user);

    logFile = fopen(logFileName, "a");
    if (logFile == NULL) {
        printf("Error opening log file!\n");
        return;
    }

    fprintf(logFile, "[%s]-%d-%s-%s\n", buffer, pid, name, status ? "JALAN" : "GAGAL");
    fclose(logFile);
}

void monitorProcess(char *user) {
    DIR *dir;
    struct dirent *ent;
    struct passwd *pwd;
    uid_t userId;

    pwd = getpwnam(user);
    if (pwd == NULL) {
        printf("User not found\n");
        return;
    }

    userId = pwd->pw_uid;

    dir = opendir("/proc");
    if (dir == NULL) {
        printf("Error opening /proc directory\n");
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            char path[PATH_MAX];
            sprintf(path, "/proc/%s/comm", ent->d_name);
            FILE *file = fopen(path, "r");
            if (file != NULL) {
                char process_name[256];
                fgets(process_name, sizeof(process_name), file);
                process_name[strcspn(process_name, "\n")] = '\0';
                int pid = atoi(ent->d_name);
                char status_path[PATH_MAX];
                sprintf(status_path, "/proc/%s/status", ent->d_name);
                FILE *status_file = fopen(status_path, "r");
                if (status_file != NULL) {
                    char line[256];
                    uid_t processUserId = -1;
                    while (fgets(line, sizeof(line), status_file)) {
                        if (strstr(line, "Uid:") != NULL) {
                            sscanf(line, "Uid: %d", &processUserId);
                            break;
                        }
                    }
                    fclose(status_file);
                    if (processUserId == userId) {
                        logProcess(user, pid, process_name, 1);
                    }
                }
                fclose(file);
            }
        }
    }

    closedir(dir);
}

void listProcesses(char *user) {
    DIR *dir;
    struct dirent *ent;
    struct passwd *pwd;
    uid_t userId;
    int found = 0;

    pwd = getpwnam(user);
    if (pwd == NULL) {
        printf("User not found\n");
        return;
    }

    userId = pwd->pw_uid;

    dir = opendir("/proc");
    if (dir == NULL) {
        printf("Error opening /proc directory\n");
        return;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            char path[PATH_MAX];
            sprintf(path, "/proc/%s/comm", ent->d_name);
            FILE *file = fopen(path, "r");
            if (file != NULL) {
                char process_name[256];
                fgets(process_name, sizeof(process_name), file);
                process_name[strcspn(process_name, "\n")] = '\0';
                int pid = atoi(ent->d_name);
                char status_path[PATH_MAX];
                sprintf(status_path, "/proc/%s/status", ent->d_name);
                FILE *status_file = fopen(status_path, "r");
                if (status_file != NULL) {
                    char line[256];
                    uid_t processUserId = -1;
                    while (fgets(line, sizeof(line), status_file)) {
                        if (strstr(line, "Uid:") != NULL) {
                            sscanf(line, "Uid: %d", &processUserId);
                            break;
                        }
                    }
                    fclose(status_file);
                    if (processUserId == userId) {
                        printf("%d\t%s\n", pid, process_name);
                        found = 1;
                    }
                }
                fclose(file);
            }
        }
    }

    closedir(dir);

    if (!found) {
        printf("No processes found for user %s\n", user);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <user>\n", argv[0]);
        printf("Options:\n");
        printf("  -m: Monitor processes\n");
        return 1;
    }

    char *user = argv[1];

    if (argc == 2) {
        listProcesses(user);
        return 0;
    }

    if (strcmp(argv[2], "-m") == 0) {
        while (1) {
            monitorProcess(user);
            sleep(1);
        }
    } else {
        printf("Invalid option!\n");
        return 1;
    }

    return 0;
}
