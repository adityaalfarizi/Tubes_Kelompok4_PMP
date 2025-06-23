#include <stdio.h>
#include <string.h>
#include "dokter.h"
#include "jadwal.h"

Dokter daftar_dokter[MAX_DOKTER];
int jumlah_dokter = 0;
Jadwal jadwal_30hari[TOTAL_SHIFT];

void tampilkan_data(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Gagal membuka file");
        return;
    }

    char baris[256];
    int is_header = 1; // flag untuk menandai baris pertama (header)
    printf("\n==========================================\n");
    printf("           DAFTAR DOKTER TERDAFTAR        \n");
    printf("==========================================\n");
    printf("| %-20s | %-10s | %-10s |\n", "Nama Dokter", "Max Shift", "Pref Shift");
    printf("--------------------------------------------------------------\n");
    while (fgets(baris, sizeof(baris), file)) {
        if (is_header) {
            is_header = 0; // Lewati header, tidak ditampilkan
            continue;
        }
        char *nama = strtok(baris, ";");
        char *max_shift = strtok(NULL, ";");
        char *pref_shift = strtok(NULL, ";\n");

        if (nama && max_shift && pref_shift) {
            printf("| %-20s | %-10s | %-10s |\n", nama, max_shift, pref_shift);
        }
    }

    fclose(file);
}

void tambah_data(const char *filename, const char *nama, int max_shift, const char *pref_shift) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Gagal membuka file");
        return;
    }

    fprintf(file, "%s;%d;%s\n", nama, max_shift, pref_shift);
    fclose(file);
    printf("Data berhasil ditambahkan.\n");
}

void hapus_data(const char *filename, const char *nama_dokter) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Gagal membuka file");
        return;
    }

    FILE *temp = fopen("temp.csv", "w");
    if (!temp) {
        perror("Gagal membuat file sementara");
        fclose(file);
        return;
    }

    char baris[256];
    int ditemukan = 0;

    // Tulis header ke file temp terlebih dahulu
    if (fgets(baris, sizeof(baris), file)) {
        fprintf(temp, "%s", baris);
    } else {
        // File kosong, tidak ada yang perlu dilakukan
        fclose(file);
        fclose(temp);
        remove("temp.csv");
        return;
    }

    // Baca sisa file
    while (fgets(baris, sizeof(baris), file)) {
        // Buat salinan baris asli, karena strtok akan mengubahnya
        char baris_salinan[256];
        strcpy(baris_salinan, baris);

        // Ambil token pertama (nama) dari salinan
        char *nama_dari_file = strtok(baris_salinan, ";");

        // Bandingkan nama dari file dengan nama yang ingin dihapus
        if (nama_dari_file != NULL && strcmp(nama_dari_file, nama_dokter) == 0) {
            ditemukan = 1; // Ditemukan! Lewati baris ini (jangan tulis ke temp)
        } else {
            fprintf(temp, "%s", baris); // Tidak cocok, tulis baris asli ke temp
        }
    }

    fclose(file);
    fclose(temp);

    if (ditemukan) {
        remove(filename); // Hapus file lama
        rename("temp.csv", filename); // Ganti nama file temp menjadi nama file lama
        printf("Data dokter '%s' berhasil dihapus.\n", nama_dokter);
    } else {
        remove("temp.csv"); // Hapus file temp karena tidak ada data yang dihapus
        printf("Data dokter '%s' tidak ditemukan.\n", nama_dokter);
    }
}