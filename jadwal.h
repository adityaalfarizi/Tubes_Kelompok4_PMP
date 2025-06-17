#ifndef JADWAL_H
#define JADWAL_H

#include "dokter.h"

void generate_jadwal(const char *filename);
void tampilkan_jadwal_harian(int hari);
void tampilkan_jadwal_mingguan(int minggu);
void tampilkan_jadwal_bulanan();
void info_shift_dokter();
void info_pelanggaran_preferensi();

#endif