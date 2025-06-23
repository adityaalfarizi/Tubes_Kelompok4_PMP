#ifndef DOKTER_H
#define DOKTER_H

#define MAX_NAMA 50
#define MAX_DOKTER 100
#define TOTAL_SHIFT 90 // 30 hari * 3 shift
#define MAX_DOKTER_PER_SHIFT 3

typedef enum { 
    PAGI = 0, 
    SIANG = 1, 
    MALAM = 2 
} ShiftType;

typedef struct {
    char nama[MAX_NAMA];
    int max_shift_per_minggu;
    ShiftType preferensi;
    int total_shift;
    int shift_terlanggar; // jumlah preferensi tidak terpenuhi
} Dokter;

typedef struct {
    int hari;
    int bulan;
    int tahun;
    ShiftType shift;
    Dokter* dokter[MAX_DOKTER_PER_SHIFT];
    int jumlah_dokter; 
} Jadwal;

extern Dokter daftar_dokter[MAX_DOKTER];
extern int jumlah_dokter;
extern Jadwal jadwal_30hari[TOTAL_SHIFT];

void tampilkan_data(const char *filename);
void tambah_data(const char *filename, const char *nama, int max_shift, const char *pref_shift);
void hapus_data(const char *filename, const char *nama_dokter);


#endif