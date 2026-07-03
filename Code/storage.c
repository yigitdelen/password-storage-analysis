#include <stdio.h>
#include <string.h>
#include <zlib.h>

#define MAX_LINE 256

extern void md5_string(const char *input, char output[33]);

void store_raw(const char *processed_path, const char *raw_out_path) {
    FILE *in  = fopen(processed_path, "r");
    FILE *out = fopen(raw_out_path,   "w");
    if (!in || !out) { perror("store_raw"); return; }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), in))
        fputs(line, out);

    fclose(in); fclose(out);
    printf("[storage] Raw written.\n");
}

extern void md5_string(const char *input, char output[33]);
extern void sha256_string(const char *input, char output[65]);

void store_hashed(const char *processed_path, const char *hashed_out_path) {
    FILE *in  = fopen(processed_path,  "r");
    FILE *out = fopen(hashed_out_path, "w");
    if (!in || !out) { perror("store_hashed"); return; }

    char line[MAX_LINE], md5[33], sha256[65];
    while (fgets(line, sizeof(line), in)) {
        line[strcspn(line, "\r\n")] = 0;
        md5_string(line, md5);
        sha256_string(line, sha256);
        fprintf(out, "%s %s\n", md5, sha256);
    }

    fclose(in); fclose(out);
    printf("[storage] Hashed written.\n");
}

void store_compressed(const char *processed_path, const char *compressed_out_path) {
    FILE  *in  = fopen(processed_path, "rb");
    if (!in) { perror("store_compressed"); return; }

    gzFile out = gzopen(compressed_out_path, "wb9");
    if (!out) { fclose(in); return; }

    char buf[4096];
    int  n;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
        gzwrite(out, buf, n);

    fclose(in);
    gzclose(out);
    printf("[storage] Compressed written.\n");
}