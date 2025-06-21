#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dokter.h"


void generate_jadwal(const char *filename) {
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Gagal membuka file CSV");
        return;
    }

    char baris[256];
    fgets(baris, sizeof(baris), file); // Lewati baris header
    jumlah_dokter = 0; // Reset jumlah dokter setiap kali generate jadwal baru

    // Inisialisasi ulang statistik dokter sebelum memuat
    for (int i = 0; i < MAX_DOKTER; i++) {
        daftar_dokter[i].total_shift = 0;
        daftar_dokter[i].shift_terlanggar = 0;
    }

    // Baca data dokter dari file CSV
    while (fgets(baris, sizeof(baris), file)) {
        if (jumlah_dokter >= MAX_DOKTER) break;

        // Membersihkan newline di akhir baris jika ada
        baris[strcspn(baris, "\r\n")] = 0;

        char *nama = strtok(baris, ";");
        char *max_shift_str = strtok(NULL, ";");
        char *pref_str = strtok(NULL, ";");

        if (nama && max_shift_str && pref_str) {
            strncpy(daftar_dokter[jumlah_dokter].nama, nama, MAX_NAMA - 1);
            daftar_dokter[jumlah_dokter].nama[MAX_NAMA - 1] = '\0';

            daftar_dokter[jumlah_dokter].max_shift_per_minggu = atoi(max_shift_str);

            if (strcmp(pref_str, "PAGI") == 0)
                daftar_dokter[jumlah_dokter].preferensi = PAGI;
            else if (strcmp(pref_str, "SIANG") == 0)
                daftar_dokter[jumlah_dokter].preferensi = SIANG;
            else if (strcmp(pref_str, "MALAM") == 0)
                daftar_dokter[jumlah_dokter].preferensi = MALAM;
            else
                continue; // Lewati jika preferensi tidak valid

            jumlah_dokter++;
        }
    }
    fclose(file);

    // Pemeriksaan setelah mencoba memuat data
    if (jumlah_dokter == 0) {
        printf("Tidak ada data dokter yang berhasil dimuat dari file.\n");
        return;
    }

    // Inisialisasi jadwal
    for (int i = 0; i < TOTAL_SHIFT; i++) {
        jadwal_30hari[i].dokter = NULL;
        jadwal_30hari[i].hari = (i / 3) + 1;
        jadwal_30hari[i].bulan = 6;
        jadwal_30hari[i].tahun = 2025;
        jadwal_30hari[i].shift = (ShiftType)(i % 3);
    }

    for (int minggu = 0; minggu < 5; minggu++) { // 4 minggu penuh + 1 minggu parsial (hari 29-30)
        int start_shift = minggu * 21;
        int end_shift = (minggu + 1) * 21;
        if (end_shift > TOTAL_SHIFT) end_shift = TOTAL_SHIFT;

        // Pertama, alokasikan semua shift preferensi terlebih dahulu
        for (int shift_type = PAGI; shift_type <= MALAM; shift_type++) {
            for (int i = start_shift; i < end_shift; i++) {
                if (jadwal_30hari[i].shift == (ShiftType)shift_type && jadwal_30hari[i].dokter == NULL) {
                    // Cari dokter dengan preferensi yang cocok
                    for (int j = 0; j < jumlah_dokter; j++) {
                        Dokter *d = &daftar_dokter[j];
                        
                        if (d->preferensi == (ShiftType)shift_type) {
                            // Hitung shift yang sudah diambil dokter ini minggu ini
                            int shift_minggu_ini = 0;
                            for (int k = start_shift; k < end_shift && k < TOTAL_SHIFT; k++) {
                                if (jadwal_30hari[k].dokter == d) {
                                    shift_minggu_ini++;
                                }
                            }
                            
                            // Jika masih bisa mengambil shift (untuk minggu parsial, anggap max_shift berlaku proporsional)
                            int max_allowed = (end_shift - start_shift) < 21 ? 
                                (d->max_shift_per_minggu * (end_shift - start_shift) + 20) / 21 : // Pembulatan ke atas
                                d->max_shift_per_minggu;
                                
                            if (shift_minggu_ini < max_allowed) {
                                jadwal_30hari[i].dokter = d;
                                d->total_shift++;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Kemudian, isi shift yang masih kosong dengan dokter lain yang tersedia
        for (int i = start_shift; i < end_shift; i++) {
            if (jadwal_30hari[i].dokter == NULL) {
                for (int j = 0; j < jumlah_dokter; j++) {
                    Dokter *d = &daftar_dokter[j];
                    
                    // Hitung shift yang sudah diambil dokter ini minggu ini
                    int shift_minggu_ini = 0;
                    for (int k = start_shift; k < end_shift && k < TOTAL_SHIFT; k++) {
                        if (jadwal_30hari[k].dokter == d) {
                            shift_minggu_ini++;
                        }
                    }
                    
                    // Hitung maksimal shift yang diizinkan (proporsional untuk minggu parsial)
                    int max_allowed = (end_shift - start_shift) < 21 ? 
                        (d->max_shift_per_minggu * (end_shift - start_shift) + 20) / 21 : // Pembulatan ke atas
                        d->max_shift_per_minggu;
                    
                    // Jika masih bisa mengambil shift
                    if (shift_minggu_ini < max_allowed) {
                        jadwal_30hari[i].dokter = d;
                        d->total_shift++;
                        if (d->preferensi != jadwal_30hari[i].shift) {
                            d->shift_terlanggar++;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void tampilkan_jadwal_harian(int hari) {
    if (hari < 1 || hari > 30) {
        printf("Hari harus antara 1-30\n");
        return;
    }

    printf("\nJadwal Hari ke-%d:\n", hari);
    printf("-------------------------------\n");
    printf("Shift     | Dokter\n");
    printf("-------------------------------\n");

    const char* shift_names[] = {"Pagi", "Siang", "Malam"};
    for (int i = 0; i < 3; i++) {
        int idx = (hari - 1) * 3 + i;
        printf("%-9s | %s\n", shift_names[i], jadwal_30hari[idx].dokter ? jadwal_30hari[idx].dokter->nama : "(Kosong)");
    }
    printf("-------------------------------\n");
}

void tampilkan_jadwal_mingguan(int minggu) {
    if (minggu < 1 || minggu > 4) {
        printf("Minggu harus antara 1-4\n");
        return;
    }
    printf("\nJadwal Mingguan ke-%d:\n", minggu);
    printf("-------------------------------------------------\n");
    printf("Hari | Shift     | Dokter\n");
    printf("-------------------------------------------------\n");

    const char* shift_names[] = {"Pagi", "Siang", "Malam"};
    for (int h = (minggu - 1) * 7 + 1; h <= minggu * 7 && h <= 30; h++) {
        for (int i = 0; i < 3; i++) {
            int idx = (h - 1) * 3 + i;
            printf("%4d | %-9s | %s\n", h, shift_names[i], jadwal_30hari[idx].dokter ? jadwal_30hari[idx].dokter->nama : "(Kosong)");
        }
    }
    printf("-------------------------------------------------\n");
}

void tampilkan_jadwal_bulanan() {
    printf("\nJadwal Bulanan (30 Hari):\n");
    printf("-------------------------------------------------\n");
    printf("Hari | Shift     | Dokter\n");
    printf("-------------------------------------------------\n");

    const char* shift_names[] = {"Pagi", "Siang", "Malam"};
    for (int h = 1; h <= 30; h++) {
        for (int i = 0; i < 3; i++) {
            int idx = (h - 1) * 3 + i;
            printf("%4d | %-9s | %s\n", h, shift_names[i], jadwal_30hari[idx].dokter ? jadwal_30hari[idx].dokter->nama : "(Kosong)");
        }
        if (h % 7 == 0) { // Tambahkan pemisah per minggu agar lebih mudah dibaca
             printf("-------------------------------------------------\n");
        }
    }
    printf("-------------------------------------------------\n");
}

void info_shift_dokter() {
    printf("\nInfo Shift Dokter:\n");
    printf("---------------------------------------\n");
    printf("Nama                 | Total Shift\n");
    printf("---------------------------------------\n");
    for (int i = 0; i < jumlah_dokter; i++) {
        printf("%-20s | %d\n", daftar_dokter[i].nama, daftar_dokter[i].total_shift);
    }
    printf("---------------------------------------\n");
}

void info_pelanggaran_preferensi() {
    printf("\nInfo Pelanggaran Preferensi:\n");
    printf("---------------------------------------\n");
    printf("Nama                 | Pelanggaran\n");
    printf("---------------------------------------\n");
    for (int i = 0; i < jumlah_dokter; i++) {
        printf("%-20s | %d\n", daftar_dokter[i].nama, daftar_dokter[i].shift_terlanggar);
    }
    printf("---------------------------------------\n");
}
