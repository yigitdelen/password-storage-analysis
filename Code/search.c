#include <stdio.h>
#include <string.h>
#include <zlib.h>

#define MAX_LINE 256

extern void md5_string(const char *input, char output[33]);

int search_raw(const char *path, const char *target) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, target) == 0) { fclose(f); return 1; }
    }
    fclose(f); return 0;
}

extern void md5_string(const char *input, char output[33]);
extern void sha256_string(const char *input, char output[65]);

int search_hashed(const char *path, const char *target) {
    char md5[33], sha256[65];
    md5_string(target, md5);
    sha256_string(target, sha256);

    FILE *f = fopen(path, "r");
    if (!f) return -1;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\r\n")] = 0;
        // Satirda md5 ve sha256 var, md5 ile karsilastir
        char stored_md5[33];
        strncpy(stored_md5, line, 32);
        stored_md5[32] = 0;
        if (strcmp(stored_md5, md5) == 0) { fclose(f); return 1; }
    }
    fclose(f); return 0;
}

int search_compressed(const char *path, const char *target) {
    gzFile f = gzopen(path, "rb");
    if (!f) return -1;
    char line[MAX_LINE];
    while (gzgets(f, line, sizeof(line))) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, target) == 0) { gzclose(f); return 1; }
    }
    gzclose(f); return 0;
}