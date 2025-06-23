#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileio.h"
#include "dokter.h"
#include "jadwal.h"

void simpan_jadwal_ke_file(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Gagal membuka file");
        return;
    }
    
    fprintf(file, "Hari,Shift,Dokter\n");
    
    const char* shift_names[] = {"PAGI", "SIANG", "MALAM"};
    
    for (int i = 0; i < TOTAL_SHIFT; i++) {
        int hari = (i / 3) + 1;
        int shift = i % 3;
        
        fprintf(file, "%d,%s,", hari, shift_names[shift]);
    if (jadwal_30hari[i].jumlah_dokter == 0) {
        fprintf(file, "NULL\n");
    } else {
        for (int j = 0; j < jadwal_30hari[i].jumlah_dokter; j++) {
            if (j > 0) fprintf(file, ";");
            fprintf(file, "%s", jadwal_30hari[i].dokter[j]->nama);
        }
        fprintf(file, "\n");
    }
    
    fclose(file);
    }
}