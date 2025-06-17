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
        if (jadwal_30hari[i].dokter != NULL) {
            fprintf(file, "%s\n", jadwal_30hari[i].dokter->nama);
        } else {
            fprintf(file, "NULL\n");
        }
    }
    
    fclose(file);
}