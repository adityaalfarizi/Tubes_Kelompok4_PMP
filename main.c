#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dokter.h"
#include "jadwal.h"
#include "fileio.h"
#include "sleep.h"

// Fungsi untuk menampilkan menu utama (manajemen data dokter)
void tampilkan_menu_utama() {
    printf("\n--- MENU DATA DOKTER ---\n");
    printf("1. Tambah Dokter\n");
    printf("2. Hapus Dokter\n");
    printf("3. Tampilkan Daftar Dokter\n");
    printf("4. Masuk ke Menu Pengaturan Jadwal\n");
    printf("0. Keluar\n");
    printf("Pilihan: ");
}

// Fungsi untuk menampilkan sub-menu (manajemen jadwal)
void tampilkan_menu_jadwal() {
    printf("\n--- MENU PENGATURAN JADWAL ---\n");
    printf("1. Tampilkan Jadwal Harian\n");
    printf("2. Tampilkan Jadwal Mingguan\n");
    printf("3. Tampilkan Jadwal Bulanan\n");
    printf("4. Tampilkan Info Shift per Dokter\n");
    printf("5. Tampilkan Info Pelanggaran Preferensi\n");
    printf("6. Simpan Jadwal ke File\n");
    printf("0. Kembali ke Menu Utama\n");
    printf("Pilihan: ");
}

void bersihkan_buffer() {
    //buat bersihin input aja inimah
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    int pilihan_utama;
    char filename_dokter[100];
    char filename_jadwal[100];

    printf("Selamat datang di Sistem Penjadwalan Dokter!\n");
    while (1) {
        printf("Masukkan nama file data dokter (e.g., daftar_dokter.csv): ");
        scanf("%99s", filename_dokter);
        FILE *test1 = fopen(filename_dokter, "r");
        if (!test1) {
            fclose(test1);
            perror("Gagal membuka file, coba lagi!");
        }
        else {
            fclose(test1);
            break;
        }
        jeda(2);
    }
    while (1) {
        printf("Masukkan nama file output jadwal (e.g., jadwal.csv): ");
        scanf("%99s", filename_jadwal);
        FILE *test2 = fopen(filename_jadwal, "r");
        if (!test2) {
            fclose(test2);
            perror("Gagal membuka file, coba lagi!");
        }
        else {
            fclose(test2);
            break;
        }
        jeda(2);
    }

    // Loop untuk menu utama
    do {
        tampilkan_menu_utama();
        //Periksa apakah input adalah angka yang valid
        if (scanf("%d", &pilihan_utama) != 1) {
            pilihan_utama = -1; // Set ke nilai invalid agar masuk ke default
        }
        bersihkan_buffer();

        switch (pilihan_utama) {
            case 1: { // Tambah Dokter
                char nama[MAX_NAMA];
                char pref_shift[20];
                int max_shift;

                printf("  -> Nama Dokter: ");
                scanf(" %99[^\n]", nama);
                bersihkan_buffer();
                

                printf("  -> Maksimal shift per minggu: ");
                scanf("%d", &max_shift);
                bersihkan_buffer();
                

                printf("  -> Preferensi shift (PAGI/SIANG/MALAM): ");
                scanf(" %19[^\n]", pref_shift);
                string_to_upper(pref_shift);
                bersihkan_buffer();
                

                tambah_data(filename_dokter, nama, max_shift, pref_shift);
                jeda(1);
                break;
            }
            case 2: { // Hapus Dokter
                char nama_dokter[MAX_NAMA];
                printf("  -> Nama Dokter yang akan dihapus: ");
                scanf(" %99[^\n]", nama_dokter);
                bersihkan_buffer();
                
                hapus_data(filename_dokter, nama_dokter);
                jeda(1);
                break;
            }
            case 3: { // Tampilkan Dokter
                tampilkan_data(filename_dokter);
                jeda(1);
                break;
            }
            case 4: { // Masuk ke Menu Jadwal
                printf("\nMemuat data dokter dan men-generate jadwal...\n");
                generate_jadwal(filename_dokter);
                jeda(3);

                int pilihan_jadwal;
                // Loop untuk sub-menu jadwal
                do {
                    tampilkan_menu_jadwal();
                    scanf("%d", &pilihan_jadwal);
                    

                    switch (pilihan_jadwal) {
                        case 1: { // Tampilkan Jadwal Harian
                            int hari;
                            printf("  -> Hari ke (1-30): ");
                            scanf("%d", &hari);
                            bersihkan_buffer();
                            
                            tampilkan_jadwal_harian(hari);
                            jeda(1);
                            break;
                        }
                        case 2: { // Tampilkan Jadwal Mingguan
                            int minggu;
                            printf("  -> Minggu ke (1-4): ");
                            scanf("%d", &minggu);
                            bersihkan_buffer();
                            
                            tampilkan_jadwal_mingguan(minggu);
                            jeda(1);
                            break;
                        }
                        case 3: // Tampilkan Jadwal Bulanan
                            tampilkan_jadwal_bulanan();
                            jeda(1);
                            break;
                        case 4: // Info Shift Dokter
                            info_shift_dokter();
                            jeda(1);
                            break;
                        case 5: // Info Pelanggaran Preferensi
                            info_pelanggaran_preferensi();
                            jeda(1);
                            break;
                        case 6: { // Simpan Jadwal ke File
                            simpan_jadwal_ke_file(filename_jadwal);
                            printf("Jadwal telah disimpan ke file '%s'\n", filename_jadwal);
                            bersihkan_buffer();
                            jeda(1);
                            break;
                        }
                        case 0:
                            printf("Kembali ke Menu Utama...\n");
                            jeda(2);
                            break;
                        default:
                            printf("Pilihan tidak valid!\n");
                            jeda(1);
                            break;
                    }
                } while (pilihan_jadwal != 0);
                break;
            }
            case 0:
                printf("Terima kasih telah menggunakan program ini. Keluar...\n");
                jeda(4);
                break;
            default:
                printf("Pilihan tidak valid!\n");
                jeda(1);
                break;
        }
    } while (pilihan_utama != 0);

    return 0;
}