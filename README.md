# LAPORAN RESMI SOAL SHIFT MODUL 2 SISTEM OPERASI 2024
## ANGGOTA KELOMPOK IT08

1. Naufal Syafi' Hakim          (5027231022)
2. RM. Novian Malcolm Bayuputra (5027231035)
3. Abid Ubaidillah Adam         (5027231089)

## SOAL NOMOR 1
### virus.c
Dalam soal ini, program diminta untuk mengganti 'suspicious string' pada text dalam direktory yang dituju, lalu mencatatnya dalam logfile bernama `virus.log`. Berikut adalah penjelasan dari kode virus.c yang dibuat.
### 1. Header file & define 
Header include pada kode dan define nama LOG_FILE sesuai soal yaitu `virus.log`
```bash
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
```
### 2. replaceStrings
Fungsi ini digunakan untuk mengganti suspicious strings(contohnya ada di variabel `patterns[]`) dalam for loop di fungsi ini program akan mencari string yang sama sesuai variabel `patterns[]`,lalu apabila ditemukan maka program akan mengganti string tersebut dengan string yang ada di dalam variabel `replacements[]`. Setelahnya program akan terus mencari lagi mana suspicious string berikutnya untuk diganti lagi.
```bash 
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
```
### 3. logReplacements
Fungsi ini digunakan untuk menginputkan log atau catatan ke dalam filelog virus.log sesuai format:
 [dd-mm-yy][H:M:S] Suspicious string at [pathfile] successfully replaced!
```bash
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
```
### 4. processFile
Fungsi ini akan membuka filepath sesuai argumen yang diberikan oleh user lalu menjalankan proses penggantian string lalu mencari file lagi yang sekiranya terdapat suspicious string, apabila tidak ada maka program akan menutup file terakhir yang ia buka
```bash
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
```
### 5. processDirectory
Fungsi untuk mengecek apakah direktori yang diberikan user kosong atau tidak
```bash
void processDirectory(const char *dirPath) {
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        printf("Error opening directory: %s\n", dirPath);
        return;
    }
```
### 6. dirent *entry
Mengecek apakah file yang diberikan berawalan dengan `.` atau `..` yang merupakan file proses yang tidak seharusnya diganti
```bash
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
```
### 7. Main Function
Main function akan menerima 2 argumen, apabila bukan 2 maka program tidak akan berjalan. Setelah menerima argumen, program melakukan fork dan akan menjeda setiap 15 detik.
```bash
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
```
### Cara Penggunaan 
#### Compile terlebih dahulu program `virus.c`
```bash
gcc virus.c -o virus
```
#### Jalankan program dengan argumen nama filepath yang ingin dijalankan program. Disini saya menggunakan contoh direktori path saya sendiri, ubahlah sesuai path yang anda inginkan agar program berjalan dengan benar.
```bash
./virus /home/ubuntu/sisop/no1/tes
```
### Isi dari `virus.log` yang dibuat program
![IMG-20240427-WA0014](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/146155753/169b07c4-5659-4a56-a789-39935f793d28)
### File dalam path sebelum program dijalankan
![IMG-20240427-WA0013](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/146155753/0d6e3787-0015-4dfb-ae75-bf87cc384c2c)
### File dalam path setelah program dijalankan
![IMG-20240427-WA0015](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/146155753/0471297a-bfb5-4cb8-b9bc-8574862ea801)

## SOAL NOMOR 2

### management.c

**1. Fungsi `write_log' berfungsi untuk menuliskan log setiap menjalankan command**
```bash
void write_log(const char *username, const char *filename, const char *action) {
    // Get current time
    time_t rawtime;
    struct tm *info;
    char buffer[80];
    time(&rawtime);
    info = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", info);

    // Open log file for appending
    FILE *log_file = fopen("/home/ubuntu/sisop2soal2/history.log", "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    // Write log message
    fprintf(log_file, "[%s][%s] - %s - %s\n", username, buffer, filename, action);

    // Close log file
    fclose(log_file);
}
```

**2. Fungsi `is_library_exists' berfungsi untuk mengecek apakah sudah ada folder library atau belum**
```bash
int is_library_exists(const char *dir_path) {
    DIR *d = opendir(dir_path);
    if (d) {
        struct dirent *dir;
        while ((dir = readdir(d)) != NULL) {
            if (strcmp(dir->d_name, "library") == 0) {
                closedir(d);
                return 1;
            }
        }
        closedir(d);
    }
    return 0;
}
```

**3. Fungsi `download_file' berfungsi untuk mendownload file**
```bash
void download_file() {
    if (!is_library_exists("/home/ubuntu/sisop2soal2")) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Menggunakan perintah sistem wget untuk mengunduh file
            execl("/usr/bin/wget", "wget", "--no-check-certificate", "https://drive.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup", "-O", "library.zip", NULL);
            // execl hanya kembali jika terjadi kesalahan
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Download failed\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}
```

**4. Fungsi `unzip_file' berfungsi untuk mengunzip file**
```bash
void unzip_file() {
    if (!is_library_exists("/home/ubuntu/sisop2soal2")) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Menggunakan perintah sistem unzip untuk mengekstrak file
            execl("/usr/bin/unzip", "unzip", "library.zip", NULL);
            // execl hanya kembali jika terjadi kesalahan
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                fprintf(stderr, "Unzip failed\n");
                exit(EXIT_FAILURE);
            }
        }
        char zip_file_path[512] = "/home/ubuntu/sisop2soal2/library.zip";
            if (remove(zip_file_path) != 0) {
                perror("Error deleting library.zip");
            }
    }
}
```

**5. Fungsi `decrypt_filename' berfungsi untuk mendekrip nama file dengan ROT7**
```bash
void decrypt_filename(char *filename) {
    while (*filename) {
        char c = *filename;
        if ((c >= 'a' && c <= 'z')) {
            // Dekripsi huruf kecil
            *filename = ((c - 'a' + 7) % 26) + 'a';
        } else if ((c >= 'A' && c <= 'Z')) {
            // Dekripsi huruf besar
            *filename = ((c - 'A' + 7) % 26) + 'A';
        }
        filename++;
    }
}
```

**6. Fungsi `decrypt_and_rename_files' berfungsi untuk mengubah nama file dengan yang sudah didekrip**
```bash
// Fungsi untuk mendekripsi dan mengubah nama file
void decrypt_and_rename_files(const char *dir_path) {
    DIR *d;
    struct dirent *dir;

    d = opendir(dir_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // Skip file yang diawali dengan angka
            if (dir->d_name[0] >= '0' && dir->d_name[0] <= '9') {
                continue;
            }

            // Buat path lengkap untuk file lama dan baru
            char old_path[512];
            strcpy(old_path, dir_path);
            strcat(old_path, "/");
            strcat(old_path, dir->d_name);
            
            // Pastikan hanya file reguler yang diproses
            struct stat st;
            if (stat(old_path, &st) == 0 && S_ISREG(st.st_mode)) {
                // Dekripsi nama file
                decrypt_filename(dir->d_name);

                // Buat nama file baru
                char new_name[256];
                strcpy(new_name, dir->d_name);

                // Buat path baru
                char new_path[512];
                strcpy(new_path, dir_path);
                strcat(new_path, "/");
                strcat(new_path, new_name);

                // Ubah nama file
                if (rename(old_path, new_path) != 0) {
                    perror("Rename failed");
                }
                    write_log("ubuntu", new_name, "Successfully renamed.");
            }
        }

        closedir(d);
    } else {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
}
```

**7. Fungsi `get_file_extension' berfungsi untuk mendapatkan extension file**
```bash
const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}
```

**8. Fungsi `remove_files_with_code' berfungsi untuk menghapus file yang mengandung kode "d3Let3" pada nama filenya**
```bash
void remove_files_with_code(const char *dir_path, const char *code) {
    DIR *d;
    struct dirent *dir;

    d = opendir(dir_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, code) != NULL) {
                char file_path[512];
                strcpy(file_path, dir_path);
                strcat(file_path, "/");
                strcat(file_path, dir->d_name);
                if (remove(file_path) != 0) {
                    perror("Error deleting file");
                }
            }
        write_log("ubuntu", dir->d_name, "Successfully deleted.");
        }
        closedir(d);
    } else {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
}
```

**9. Fungsi `rename_file_by_extension' berfungsi untuk mengubah nama file sesuai dengan ekstensi**
```bash
// Fungsi untuk mengubah nama file sesuai dengan ekstensi
void rename_file_by_extension(const char *dir_path, const char *filename, const char *extension) {
    // Tentukan nama baru sesuai dengan ekstensi
    char new_name[512];
    if (strcmp(extension, "ts") == 0) {
        strcpy(new_name, "helper.ts");
    } else if (strcmp(extension, "py") == 0) {
        strcpy(new_name, "calculator.py");
    } else if (strcmp(extension, "go") == 0) {
        strcpy(new_name, "server.go");
    } else {
        strcpy(new_name, "renamed.file");
    }
    // Buat path lama dan baru
    char old_path[512];
    strcpy(old_path, dir_path);
    strcat(old_path, "/");
    strcat(old_path, filename);
    char new_path[512];
    strcpy(new_path, dir_path);
    strcat(new_path, "/");
    strcat(new_path, new_name);
    // Ubah nama file
    if (rename(old_path, new_path) != 0) {
        perror("Rename failed");
    }
}
```

**10. Fungsi `rename_files_by_code' berfungsi untuk merename file dengan kode "r3N4mE" pada nama filenya**
```bash
void rename_files_by_code(const char *dir_path, const char *code) {
    DIR *d;
    struct dirent *dir;

    d = opendir(dir_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, code) != NULL) {
                // Dapatkan ekstensi file
                const char *extension = get_file_extension(dir->d_name);
                // Ubah nama file sesuai dengan ekstensi
                rename_file_by_extension(dir_path, dir->d_name, extension);
            }
        }
        closedir(d);
    } else {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
}
```

**11. Fungsi `create_backup_folder' berfungsi untuk membuat folder backup jika belum ada**
```bash
void create_backup_folder(const char *backup_dir) {
    struct stat st;
    if (stat(backup_dir, &st) != 0) {
        // Folder tidak ditemukan, maka coba membuat folder baru
        if (mkdir(backup_dir, 0777) == -1) {
            perror("Error creating backup folder");
            exit(EXIT_FAILURE);
        }
    }
}
```

**12. Fungsi `backup_files' berfungsi untuk membuat backup file dengan kode "m0V3"**
```bash
void backup_files(const char *dir_path, const char *backup_dir) {
    create_backup_folder(backup_dir); // Membuat folder backup jika belum ada

    DIR *d;
    struct dirent *dir;

    d = opendir(dir_path);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (strstr(dir->d_name, "m0V3") != NULL) {
                char file_path[512];
                strcpy(file_path, dir_path);
                strcat(file_path, "/");
                strcat(file_path, dir->d_name);
                char backup_path[512];
                strcpy(backup_path, backup_dir);
                strcat(backup_path, "/");
                strcat(backup_path, dir->d_name);
                if (rename(file_path, backup_path) != 0) {
                    perror("Error moving file to backup");
                }
                write_log("ubuntu", dir->d_name, "Successfully moved to backup.");
            }
        }
        closedir(d);
    } else {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
}
```

**13. Fungsi `restore_files' berfungsi untuk mengembalikan file dari backup ke direktori asal**
```bash
void restore_files(const char *backup_dir, const char *original_dir) {
    DIR *d;
    struct dirent *dir;

    d = opendir(backup_dir);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            // Jika nama direktori adalah . atau ..
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                continue;
            }

            char file_path[512];
            strcpy(file_path, backup_dir);
            strcat(file_path, "/");
            strcat(file_path, dir->d_name);

            // Buat path untuk file di direktori asal
            char original_file_path[512];
            strcpy(original_file_path, original_dir);
            strcat(original_file_path, "/");
            strcat(original_file_path, dir->d_name);
            
            // Pindahkan file dari folder backup ke folder asal
            if (rename(file_path, original_file_path) != 0) {
                perror("Error moving file from backup");
            }
            write_log("ubuntu", dir->d_name, "Successfully restored from backup.");
        }
        closedir(d);
    } else {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }
}
```

**Fungsi `int main'**
```bash
int main(int argc, char *argv[]) {
    if (argc < 2) {
        // Default mode: rename dan hapus file
        download_file();
        unzip_file();
        decrypt_and_rename_files("/home/ubuntu/sisop2soal2/library");
        remove_files_with_code("/home/ubuntu/sisop2soal2/library", "d3Let3");
        rename_files_by_code("/home/ubuntu/sisop2soal2/library", "r3N4mE");
    } else {
        // Mode tambahan
        if (strcmp(argv[1], "-m") == 0) {
            if (argc < 3) {
                fprintf(stderr, "Usage: %s -m <mode>\n", argv[0]);
                exit(EXIT_FAILURE);
            }
            if (strcmp(argv[2], "backup") == 0) {
                // Mode backup: pindahkan file dengan kode "m0V3" ke folder "backup"
                backup_files("/home/ubuntu/sisop2soal2/library", "/home/ubuntu/sisop2soal2/library/backup");
            } else if (strcmp(argv[2], "restore") == 0) {
                // Mode restore: kembalikan file dari folder "backup" ke folder asal
                restore_files("/home/ubuntu/sisop2soal2/library/backup", "/home/ubuntu/sisop2soal2/library");
            } else {
                fprintf(stderr, "Unknown mode: %s\n", argv[2]);
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
```

**Hasil setelah menjalankan `./management`**

![Screenshot from 2024-04-27 23-15-36](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/ad3b6f58-22cb-448e-9a0e-1ec5c96f0abf)

![image](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/ca7b18b6-c45a-4841-855f-4351f8fe2a9f)

**Hasil setelah menjalankan `./management -m backup`**

![image](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/4c624794-25ab-4554-b8af-c404e72e9e00)

![image](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/3b152e4f-c24f-4c15-b426-ece04d6d57bd)

**Hasil setelah menjalankan `./management -m restore`**

![image](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/92ea1ade-fd67-402f-9b50-69855b3906a7)

![image](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/6ea5760c-67e8-4353-8d47-894d12d34f40)

**Isi dari `history.log`**

![Screenshot from 2024-04-27 23-49-19](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/2b8a1e94-7c68-4e62-ae9e-25a4a718822b)

![Screenshot from 2024-04-27 23-49-30](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/4840c8ef-3769-4fac-89a5-0ecbf9cf3c12)

![Screenshot from 2024-04-27 23-49-36](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/122516105/112ba2fd-8a34-402c-9912-841d3636141b)

Kendala : Program belum berjalan secara daemon dan tidak bisa menggunakan signal

## SOAL NOMOR 3
Program nomor 3 menerima input argumen ketika dijalankan sebagai fitur dari program ini. Berikut penjelasannya.
### 1. logProcess
Fungsi ini digunakan program apabila user ingin menggunakan fitur `-m` yaitu monitoring process dimana fungsi ini akan menyimpan log atau catatan monitoring proses kedalam file <user>.log dengan format :
[dd:mm:yy-H:M:S]-pid-program-status
```bash
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
```
### 2. monitorProcess
Fungsi untuk memonitoring proses apabila user memberikan argumen `-m`
```bash

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

    while (1) {
        rewinddir(dir);
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
        sleep(1);
    }

    closedir(dir);
}
```
### 3. listProcess
Fungsi untuk melaksanakan argumen `-l` yaitu program akan menampilkan list semua kegiatan user. 
```bash
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
```
### 4. Main Function
Main function menerima 3 argumen apabila tidak maka program tidak berjalan. Dalam hal ini saya menggunakan switch case ketika argumen yang diberikan `-l` program akan menjalankan fungsi `listProcesses`, apabila `-m` maka akan melakukan fork() dan menjalankan fungsi `monitorProcesses`, apabila `-s` maka akan meng-kill program monitoring process yang sedang berjalan. Sementara itu fitur untuk `-c` dan `-a` belum bisa saya buat karena keterbatasan waktu dan tenaga.

Note : Hati-hati dalam menggunakan fitur `-s` karena program akan membunuh proses sampai dapat menyebabkan vmware blank screen atau terlogout dari user. Dalam kasus saya selama ini sebenarnya aman saja namun mungkin saja berbahaya bagi user lain. Ini juga merupakan problem dari kode yang belum bisa saya selesaikan dengan aman untuk menghentikan program monitoring proccess.
```bash
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <option> <user>\n", argv[0]);
        printf("Options:\n");
        printf("  -l: List processes\n");
        printf("  -m: Monitor processes\n");
        printf("  -s: Stop monitoring processes\n");
        printf("  -c: Kill processes continuously\n");
        printf("  -a: Stop killing processes\n");
        return 1;
    }

    char option = argv[1][1];
    char *user = argv[2];

    switch (option) {
        case 'l':
            listProcesses(user);
            break;
        case 'm':
            child_pid = fork();
            if (child_pid == 0) {
                monitorProcess(user);
                exit(0);
            } else if (child_pid < 0) {
                printf("Failed to create child process\n");
            }
            break;
        case 's':
            if (child_pid < 0) {
                kill(child_pid, SIGTERM);
                child_pid = -1;
                printf("Monitoring processes stopped\n");
            } else {
                printf("No monitoring process found\n");
            }
            break;
        case 'c':
            // unfinished
            break;
        case 'a':
            // unfinished
            break;
        default:
            printf("Invalid option!\n");
            return 1;
    }

    return 0;
}
```

### Cara Penggunaan
1. Compile kode admin.c dengan command
```bash
gcc admin.c -o admin
```
2. Jalankan argumen yang anda inginkan, disini contohnya adalah argumen `-m` yang merupakan monitoring proccess
```bash
./admin -m <user>
```
### Fitur `-l` program
![IMG-20240427-WA0016](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/146155753/61214a57-21aa-46a4-9d45-1a5041c597e7)
### Fitur `-m` program
![IMG-20240427-WA0012](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/146155753/b07f94f4-aa10-4b4c-b6b9-6e2746708a86)
### Fitur `-s` program
Tidak dapat saya tampilkan karena virtual machine saya langsung terlogout dan blank screen sehingga tidak bisa mengambil screenshoot

## Soal Nomor 4
## Deskripsi
Program ini dirancang untuk mengelola pembukaan dan penutupan beberapa instansi dari berbagai aplikasi berdasarkan input pengguna dari baris perintah atau file konfigurasi. Ini berguna untuk pengujian, mengelola sesi aplikasi secara massal, atau mengotomatisasi setup dari beberapa instansi perangkat lunak.

## Cara Kerja
Program ini memiliki tiga fungsionalitas utama:
1. Membuka aplikasi langsung melalui argumen baris perintah
2. Membuka aplikasi dari file konfigurasi
3. Menghentikan semua aplikasi yang sedang dikelola

## Kompilasi
Untuk mengkompilasi program, gunakan perintah GCC berikut:
```bash
gcc setup.c -o setup
```
Ini akan menghasilkan eksekutabel yang bernama setup.c

## Penggunaan
Program dapat dijalankan dalam tiga mode berbeda:

### 1. Membuka Aplikasi Langsung
Untuk menentukan aplikasi dan jumlah instansi langsung melalui argumen baris saya menggunakan fungsi seperti ini:
```bash
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
```
Fungsi ini melakukan beberapa hal sebagai berikut:
1. Menghitung Jumlah Argumen: Fungsi pertama menghitung jumlah total argumen yang diterima untuk memastikan input yang diberikan sesuai format yang diharapkan.
2. Validasi Argumen: Fungsi memastikan bahwa jumlah argumen setelah -o adalah genap, karena setiap aplikasi harus diikuti oleh angka yang menyatakan berapa kali aplikasi tersebut harus dibuka. Jika jumlah argumen tidak sesuai, program akan menampilkan pesan penggunaan yang benar dan keluar dengan exit(EXIT_FAILURE).
3. Membuka Aplikasi: Menggunakan loop, fungsi mengulangi setiap pasangan argumen yang dimulai dari indeks 2 (indeks 0 adalah nama program, dan indeks 1 adalah flag -o). Setiap pasangan terdiri dari nama aplikasi dan jumlah jendela (atau instans) yang harus dibuka. Fungsi mengkonversi jumlah jendela dari string ke integer menggunakan atoi, kemudian memanggil open_apps_helper untuk setiap aplikasi dengan jumlah yang sesuai.

cmd perintah yang digunakan untuk mengeksekusi fungsi di atas:
```bash
./setup.c -o <app1> <num1> [<app2> <num2> ...]
```
contohnya jika ingin membuka 2 aplikasi firefox dan 2 aplikasi wireshark sekaligus, maka bentuk pengimplementasiaanyya adalah:
```bash
./setup.c -o firefox 2 wireshark 2
```
maka otuput yang akan dihasilkan seperti di bawah ini
![Screenshot 2024-04-27 210507](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/150356339/eca90c50-9646-4a49-832e-943c9760222c)
berarti fungsi telah berhasil dijalankan
### 2. Membuka Aplikasi dari File Konfigurasi
Untuk membaca file konfigurasi di mana setiap baris berisi nama aplikasi dan jumlah instansi:
```bash
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
```
dengan menggunakan command di bawah ini, maka akan mengeksekusi fungi
```bash
./setup.c -f file.con
```
![Screenshot 2024-04-27 212448](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/150356339/6e36de71-1780-4d1b-a043-40ff898f5a65)

### 3. Menghentikan Semua Aplikasi
Untuk menghentikan semua aplikasi yang dibuka oleh program ini:
```bash
void kill_all_apps() {
    for (int i = 0; i < child_pid_count; i++) {
        if (kill(child_pids[i], SIGTERM) == -1) {
            perror("Error sending SIGTERM to child process");
        }
    }
}

```
kemudian menggunakan perintah di bawah ini untuk mengeksekusi:
```bash
./setup.c -k
```

## Detail Implementasi
- **\`child_pids\` array**: Menyimpan ID proses dari semua proses anak yang dibuat.
- **\`child_pid_count\`**: Menghitung jumlah proses anak yang aktif.
## Batasan
Program mengasumsikan semua aplikasi yang ditentukan dapat dijalankan dari terminal menggunakan namanya.
## Kendala
Setelah saya menjalankan
```bash
./setup.c -o <app1> <num1> [<app2> <num2> ...]
```
atau
```bash
./setup.c -f file.con
```
selama aplikasi terbuka prompting terminal tidak muncul kembali hingga saya menutup manual aplikasi tersebut, akibatnya saya tidak bisa memasukkan command selanjutnya termasuk
```bash
./setup.c -k
```
untuk menutup apilikas aplikasi yang telah terbuka
