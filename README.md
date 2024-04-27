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
contohnya saya ingin membuka 2 aplikasi firefox dan 2 aplikasi wireshark sekaligus, maka bentuk pengimplementasiaanyya adalah:
```bash
./setup.c -o firefox 2 wireshark 2
```
maka otuput yang akan dihasilkan seperti di bawah ini
![Screenshot 2024-04-27 210507](https://github.com/rmnovianmalcolmb/Sisop-2-2024-MH-IT08/assets/150356339/eca90c50-9646-4a49-832e-943c9760222c)
berarti fungsi telah berhasil dijalankan
### 2. Membuka Aplikasi dari File Konfigurasi
Untuk membaca file konfigurasi di mana setiap baris berisi nama aplikasi dan jumlah instansi saya membuat
yang akan dibuka:
```bash
./setup.c -f file.con
```
### 3. Menghentikan Semua Aplikasi
Untuk menghentikan semua aplikasi yang dibuka oleh program ini:
```bash
./setup.c -k
```

## Detail Implementasi
- **\`child_pids\` array**: Menyimpan ID proses dari semua proses anak yang dibuat.
- **\`child_pid_count\`**: Menghitung jumlah proses anak yang aktif.

##Catatan
Namun dalam penerapan code diatas, code pada program tidak bisa mengimplementasikan penggunaan argumen -k untuk mematikan atau menutup aplikasi yang dibuka menggunakan -f atau -k

## Batasan
Program mengasumsikan semua aplikasi yang ditentukan dapat dijalankan dari terminal menggunakan namanya.


