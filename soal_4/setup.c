#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define MAX_APPS 10
#define MAX_NAME_LENGTH 50

pid_t child_pids[MAX_APPS * MAX_APPS]; // Array untuk menyimpan PID proses anak
int child_pid_count = 0; // Jumlah proses anak yang dibuat

// Deklarasi prototipe fungsi
void open_apps_helper(const char *app, int num_windows);

// Fungsi untuk membuka aplikasi berdasarkan file konfigurasi
void open_apps_from_config(const char *config_file) {
    FILE *file = fopen(config_file, "r");
    if (file == NULL) {
        perror("Error opening config file");
        exit(EXIT_FAILURE);
    }

    char app[MAX_NAME_LENGTH];
    int num_windows;
    while (fscanf(file, "%s %d", app, &num_windows) == 2) {
        open_apps_helper(app, num_windows);
    }

    fclose(file);
}

// Fungsi untuk membuka aplikasi secara langsung dari argumen baris perintah
void open_apps_directly(char *argv[]) {
    int argc = 0;
    while (argv[argc] != NULL) {
        argc++;
    }

    if ((argc - 2) % 2 != 0) {
        printf("Usage: %s -o <app1> <num1> [<app2> <num2> ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = 2; i < argc; i += 2) {
        char *app = argv[i];
        int num_windows = atoi(argv[i + 1]);
        open_apps_helper(app, num_windows);
    }
}

// Fungsi untuk menghentikan semua aplikasi yang berjalan
void kill_all_apps() {
    // Mengirim sinyal SIGTERM ke semua proses anak
    for (int i = 0; i < child_pid_count; i++) {
        if (kill(child_pids[i], SIGTERM) == -1) {
            perror("Error sending SIGTERM to child process");
        }
    }
}

int main(int argc, char *argv[]) {
    // Memeriksa jumlah argumen
    if (argc < 2) {
        printf("Usage: %s -o <app1> <num1> [<app2> <num2> ...]\n", argv[0]);
        printf("       %s -f <config_file>\n", argv[0]);
        printf("       %s -k\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Memeriksa argumen dan menjalankan fungsi yang sesuai
    if (strcmp(argv[1], "-f") == 0 && argc == 3) {
        open_apps_from_config(argv[2]);
    } else if (strcmp(argv[1], "-o") == 0) {
        open_apps_directly(argv);
    } else if (strcmp(argv[1], "-k") == 0) {
        kill_all_apps();
    } else {
        printf("Usage: %s -o <app1> <num1> [<app2> <num2> ...]\n", argv[0]);
        printf("       %s -f <config_file>\n", argv[0]);
        printf("       %s -k\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Fungsi pembantu untuk membuka aplikasi sejumlah tertentu
void open_apps_helper(const char *app, int num_windows) {
    for (int i = 0; i < num_windows; i++) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            execlp(app, app, NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        } else if (pid < 0) { // Fork error
            perror("fork");
            exit(EXIT_FAILURE);
        } else {
            // Menyimpan PID proses anak ke dalam array
            child_pids[child_pid_count++] = pid;
        }
    }
}
