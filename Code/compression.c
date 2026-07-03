#include <stdio.h>
#include <zlib.h>
#include "utils.h"

double decompress_time(const char *compressed_path) {
    double start = get_time_us();
    gzFile f = gzopen(compressed_path, "rb");
    if (!f) return -1;
    char buf[4096];
    while (gzread(f, buf, sizeof(buf)) > 0);
    gzclose(f);
    return get_time_us() - start;
}

void print_compression_ratio(const char *original_path, const char *compressed_path) {
    long orig = get_file_size(original_path);
    long comp = get_file_size(compressed_path);
    printf("Compression Ratio: %.4f  (%.2f MB -> %.2f MB)\n",
           (double)comp / orig, orig / 1e6, comp / 1e6);
}