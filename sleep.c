#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "dokter.h"
#include "jadwal.h"
#include "fileio.h"
#include "sleep.h"

void jeda(int detik) {
#ifdef _WIN32
    Sleep(detik * 1000); // Sleep() di Windows menggunakan milidetik
#else
    sleep(detik); // sleep() di sistem lain menggunakan detik
#endif
}

void string_to_upper(char *str) {
    if (str == NULL) return; // Penanganan jika string NULL
    
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}