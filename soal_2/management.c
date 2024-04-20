#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

// Fungsi untuk menulis pesan log ke file
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


// Fungsi untuk memeriksa apakah folder library sudah ada
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

// Fungsi untuk mendownload file dari URL yang diberikan
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

// Fungsi untuk mengekstrak file yang diunduh
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

// Fungsi untuk mendekripsi nama file dengan ROT7
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

// Fungsi untuk mendapatkan ekstensi file
const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

// Fungsi untuk menghapus file yang mengandung kode "d3Let3" pada nama filenya
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

// Fungsi untuk menangani file dengan kode "r3N4mE" pada nama filenya
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

// Fungsi untuk membuat folder backup jika belum ada
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

// Fungsi untuk membuat backup file dengan kode "m0V3"
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

// Fungsi untuk mengembalikan file dari backup ke direktori asal
// Fungsi untuk mengembalikan file dari backup ke direktori asal
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
