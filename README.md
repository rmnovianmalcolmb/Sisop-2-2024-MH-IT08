# LAPORAN RESMI SOAL SHIFT MODUL 2 SISTEM OPERASI 2024
## ANGGOTA KELOMPOK IT08

1. Naufal Syafi' Hakim          (5027231022)
2. RM. Novian Malcolm Bayuputra (5027231035)
3. Abid Ubaidillah Adam         (5027231089)

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

# Soal Nomor 4
## Deskripsi
Program ini dirancang untuk mengelola pembukaan dan penutupan beberapa instansi dari berbagai aplikasi berdasarkan input pengguna dari baris perintah atau file konfigurasi. Ini berguna untuk pengujian, mengelola sesi aplikasi secara massal, atau mengotomatisasi setup dari beberapa instansi perangkat lunak.

## Cara Kerja
Program ini memiliki tiga fungsionalitas utama:
1. Membuka aplikasi langsung melalui argumen baris perintah
2. Membuka aplikasi dari file konfigurasi
3. Menghentikan semua aplikasi yang sedang dikelola

## Kompilasi
Untuk mengkompilasi program, gunakan perintah GCC berikut:
\`\`\`bash
gcc -o app_manager app_manager.c
\`\`\`
Ini akan menghasilkan eksekutabel yang bernama \`app_manager\`.

## Penggunaan
Program dapat dijalankan dalam tiga mode berbeda:

### 1. Membuka Aplikasi Langsung
Untuk menentukan aplikasi dan jumlah instansi langsung melalui argumen baris perintah:
\`\`\`bash
./app_manager -o <app1> <num1> [<app2> <num2> ...]
\`\`\`
### 2. Membuka Aplikasi dari File Konfigurasi
Untuk membaca file konfigurasi di mana setiap baris berisi nama aplikasi dan jumlah instansi yang akan dibuka:
\`\`\`bash
./app_manager -f config.txt
\`\`\`
### 3. Menghentikan Semua Aplikasi
Untuk menghentikan semua aplikasi yang dibuka oleh program ini:
\`\`\`bash
./app_manager -k
\`\`\`

## Detail Implementasi
- **\`child_pids\` array**: Menyimpan ID proses dari semua proses anak yang dibuat.
- **\`child_pid_count\`**: Menghitung jumlah proses anak yang aktif.

## Penanganan Kesalahan
Penanganan kesalahan dasar untuk operasi file, pembuatan proses (\`fork\`), dan peluncuran aplikasi (\`execlp\`). Kesalahan akan dilaporkan di konsol.

## Batasan
Program mengasumsikan semua aplikasi yang ditentukan dapat dijalankan dari terminal menggunakan namanya.

## Kesimpulan
Program ini menawarkan kerangka dasar untuk manajemen aplikasi melalui terminal Linux, sehingga cocok untuk tes otomatis, manajemen sesi, atau lingkungan pengembangan.
EOF