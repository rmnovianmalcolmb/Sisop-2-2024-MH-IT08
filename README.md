
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
