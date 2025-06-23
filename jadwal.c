#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "dokter.h"

#define MAX_DOKTER_PER_SHIFT 3

void generate_jadwal(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Gagal membuka file CSV");
        return;
    }

    char baris[256];
    fgets(baris, sizeof(baris), file); // Lewati baris header
    jumlah_dokter = 0;

    // Inisialisasi ulang statistik dokter sebelum memuat
    for (int i = 0; i < MAX_DOKTER; i++) {
        daftar_dokter[i].total_shift = 0;
        daftar_dokter[i].shift_terlanggar = 0;
    }

    // Baca data dokter dari file CSV
    while (fgets(baris, sizeof(baris), file)) {
        if (jumlah_dokter >= MAX_DOKTER) break;

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
                continue;

            jumlah_dokter++;
        }
    }
    fclose(file);

    if (jumlah_dokter == 0) {
        printf("Tidak ada data dokter yang berhasil dimuat dari file.\n");
        return;
    }

    // Inisialisasi jadwal dengan multiple dokter
    for (int i = 0; i < TOTAL_SHIFT; i++) {
        jadwal_30hari[i].jumlah_dokter = 0;
        for (int j = 0; j < MAX_DOKTER_PER_SHIFT; j++) {
            jadwal_30hari[i].dokter[j] = NULL;
        }
        jadwal_30hari[i].hari = (i / 3) + 1;
        jadwal_30hari[i].bulan = 6;
        jadwal_30hari[i].tahun = 2025;
        jadwal_30hari[i].shift = (ShiftType)(i % 3);
    }

    for (int minggu = 0; minggu < 5; minggu++) {
        int start_shift = minggu * 21;
        int end_shift = (minggu + 1) * 21;
        if (end_shift > TOTAL_SHIFT) end_shift = TOTAL_SHIFT;

        // Fase 1: Isi shift sesuai preferensi dokter terlebih dahulu
        for (int shift_type = PAGI; shift_type <= MALAM; shift_type++) {
            for (int i = start_shift; i < end_shift; i++) {
                if (jadwal_30hari[i].shift == (ShiftType)shift_type && 
                    jadwal_30hari[i].jumlah_dokter < MAX_DOKTER_PER_SHIFT) {
                    
                    // Cari dokter dengan preferensi yang cocok
                    for (int j = 0; j < jumlah_dokter && jadwal_30hari[i].jumlah_dokter < MAX_DOKTER_PER_SHIFT; j++) {
                        Dokter *d = &daftar_dokter[j];
                        
                        if (d->preferensi == (ShiftType)shift_type) {
                            // Hitung shift yang sudah diambil dokter ini minggu ini
                            int shift_minggu_ini = 0;
                            for (int k = start_shift; k < end_shift; k++) {
                                for (int l = 0; l < jadwal_30hari[k].jumlah_dokter; l++) {
                                    if (jadwal_30hari[k].dokter[l] == d) {
                                        shift_minggu_ini++;
                                    }
                                }
                            }
                            
                            int max_allowed = (end_shift - start_shift) < 21 ? 
                                (d->max_shift_per_minggu * (end_shift - start_shift) + 20) / 21 : 
                                d->max_shift_per_minggu;
                                
                            if (shift_minggu_ini < max_allowed) {
                                jadwal_30hari[i].dokter[jadwal_30hari[i].jumlah_dokter] = d;
                                jadwal_30hari[i].jumlah_dokter++;
                                d->total_shift++;
                            }
                        }
                    }
                }
            }
        }

        // Fase 2: Isi shift yang masih kosong dengan dokter yang tersedia
        for (int hari = (start_shift/3)+1; hari <= (end_shift/3)+1; hari++) {
            for (int shift_type = PAGI; shift_type <= MALAM; shift_type++) {
                int idx = (hari-1)*3 + shift_type;
                if (idx >= end_shift) continue;
                
                // Jika shift ini masih kosong
                if (jadwal_30hari[idx].jumlah_dokter == 0) {
                    for (int j = 0; j < jumlah_dokter && jadwal_30hari[idx].jumlah_dokter == 0; j++) {
                        Dokter *d = &daftar_dokter[j];
                        
                        int shift_minggu_ini = 0;
                        for (int k = start_shift; k < end_shift; k++) {
                            for (int l = 0; l < jadwal_30hari[k].jumlah_dokter; l++) {
                                if (jadwal_30hari[k].dokter[l] == d) {
                                    shift_minggu_ini++;
                                }
                            }
                        }
                        
                        int max_allowed = (end_shift - start_shift) < 21 ? 
                            (d->max_shift_per_minggu * (end_shift - start_shift) + 20) / 21 : 
                            d->max_shift_per_minggu;
                            
                        if (shift_minggu_ini < max_allowed) {
                            jadwal_30hari[idx].dokter[0] = d;
                            jadwal_30hari[idx].jumlah_dokter = 1;
                            d->total_shift++;
                            d->shift_terlanggar++; // Karena tidak sesuai preferensi
                        }
                    }
                }
            }
        }

        // Fase 3: Isi slot tambahan di shift yang sudah ada dokternya
        for (int i = start_shift; i < end_shift; i++) {
            if (jadwal_30hari[i].jumlah_dokter > 0 && 
                jadwal_30hari[i].jumlah_dokter < MAX_DOKTER_PER_SHIFT) {
                
                for (int j = 0; j < jumlah_dokter && jadwal_30hari[i].jumlah_dokter < MAX_DOKTER_PER_SHIFT; j++) {
                    Dokter *d = &daftar_dokter[j];
                    
                    // Cek apakah dokter sudah ada di shift ini
                    int sudah_ada = 0;
                    for (int k = 0; k < jadwal_30hari[i].jumlah_dokter; k++) {
                        if (jadwal_30hari[i].dokter[k] == d) {
                            sudah_ada = 1;
                            break;
                        }
                    }
                    if (sudah_ada) continue;
                    
                    int shift_minggu_ini = 0;
                    for (int k = start_shift; k < end_shift; k++) {
                        for (int l = 0; l < jadwal_30hari[k].jumlah_dokter; l++) {
                            if (jadwal_30hari[k].dokter[l] == d) {
                                shift_minggu_ini++;
                            }
                        }
                    }
                    
                    int max_allowed = (end_shift - start_shift) < 21 ? 
                        (d->max_shift_per_minggu * (end_shift - start_shift) + 20) / 21 : 
                        d->max_shift_per_minggu;
                        
                    if (shift_minggu_ini < max_allowed) {
                        jadwal_30hari[i].dokter[jadwal_30hari[i].jumlah_dokter] = d;
                        jadwal_30hari[i].jumlah_dokter++;
                        d->total_shift++;
                        if (d->preferensi != jadwal_30hari[i].shift) {
                            d->shift_terlanggar++;
                        }
                    }
                }
            }
        }
    }
}


// Fungsi tampilan lainnya perlu disesuaikan untuk menangani multiple dokter
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
        printf("%-9s | ", shift_names[i]);
        if (jadwal_30hari[idx].jumlah_dokter == 0) {
            printf("(Kosong)\n");
        } else {
            for (int j = 0; j < jadwal_30hari[idx].jumlah_dokter; j++) {
                if (j > 0) printf(", ");
                printf("%s", jadwal_30hari[idx].dokter[j]->nama);
            }
            printf("\n");
        }
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
            printf("%4d | %-9s | ", h, shift_names[i]);
            
            if (jadwal_30hari[idx].jumlah_dokter == 0) {
                printf("(Kosong)\n");
            } else {
                for (int j = 0; j < jadwal_30hari[idx].jumlah_dokter; j++) {
                    if (j > 0) printf(", ");
                    printf("%s", jadwal_30hari[idx].dokter[j]->nama);
                }
                printf("\n");
            }
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
            printf("%4d | %-9s | ", h, shift_names[i]);
            
            if (jadwal_30hari[idx].jumlah_dokter == 0) {
                printf("(Kosong)\n");
            } else {
                for (int j = 0; j < jadwal_30hari[idx].jumlah_dokter; j++) {
                    if (j > 0) printf(", ");
                    printf("%s", jadwal_30hari[idx].dokter[j]->nama);
                }
                printf("\n");
            }
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