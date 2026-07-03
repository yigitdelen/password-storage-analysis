#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"
#include <windows.h>
#include <psapi.h>
#include <math.h>

void preprocess(const char *input, const char *output, const char *mode);
void store_raw(const char *processed, const char *out);
void store_hashed(const char *processed, const char *out);
void store_compressed(const char *processed, const char *out);
int  search_raw(const char *path, const char *target);
int  search_hashed(const char *path, const char *target);
int  search_compressed(const char *path, const char *target);
double decompress_time(const char *path);
void print_compression_ratio(const char *orig, const char *comp);

#define SEARCH_COUNT 1000
#define MAX_LINE     256
#define POOL_SIZE    500000

void pick_random_passwords(const char *processed_path, char passwords[][MAX_LINE], int n) {
    FILE *f = fopen(processed_path, "r");
    if (!f) { perror("pick_random"); return; }

    char **pool = malloc(POOL_SIZE * sizeof(char*));
    char line[MAX_LINE];
    int count = 0;

    while (fgets(line, sizeof(line), f) && count < POOL_SIZE) {
        line[strcspn(line, "\r\n")] = 0;
        pool[count++] = strdup(line);
    }
    fclose(f);

    srand(42);
    for (int i = 0; i < n; i++) {
        int idx = rand() % count;
        strncpy(passwords[i], pool[idx], MAX_LINE);
    }
    for (int i = 0; i < count; i++) free(pool[i]);
    free(pool);
}

void benchmark(const char *label,
               int (*search_fn)(const char*, const char*),
               const char *path,
               char passwords[][MAX_LINE], int n) {

    double total = 0, max_t = 0;
    double times[SEARCH_COUNT];

    // RAM ve CPU için
    FILETIME idle1, kernel1, user1, idle2, kernel2, user2;
    GetSystemTimes(&idle1, &kernel1, &user1);

    for (int i = 0; i < n; i++) {
        double t0      = get_time_us();
        search_fn(path, passwords[i]);
        double elapsed = get_time_us() - t0;
        times[i]       = elapsed;
        total         += elapsed;
        if (elapsed > max_t) max_t = elapsed;
    }

    GetSystemTimes(&idle2, &kernel2, &user2);
    long ram_after = get_ram_usage_mb();

    // Standart sapma
    double avg = total / n;
    double variance = 0;
    for (int i = 0; i < n; i++)
        variance += (times[i] - avg) * (times[i] - avg);
    double stddev = sqrt(variance / n);

    // CPU hesapla
    ULARGE_INTEGER u1, u2, k1, k2, i1, i2;
    u1.LowPart = user1.dwLowDateTime;   u1.HighPart = user1.dwHighDateTime;
    u2.LowPart = user2.dwLowDateTime;   u2.HighPart = user2.dwHighDateTime;
    k1.LowPart = kernel1.dwLowDateTime; k1.HighPart = kernel1.dwHighDateTime;
    k2.LowPart = kernel2.dwLowDateTime; k2.HighPart = kernel2.dwHighDateTime;
    i1.LowPart = idle1.dwLowDateTime;   i1.HighPart = idle1.dwHighDateTime;
    i2.LowPart = idle2.dwLowDateTime;   i2.HighPart = idle2.dwHighDateTime;

    double total_time = (double)((k2.QuadPart - k1.QuadPart) + (u2.QuadPart - u1.QuadPart));
    double idle_time  = (double)(i2.QuadPart - i1.QuadPart);
    double cpu = (total_time == 0) ? 0 : (1.0 - idle_time / total_time) * 100.0;

    printf("\n[%s]\n", label);
    printf("  Average Search Time : %8.2f µs\n", avg);
    printf("  Std Deviation       : %8.2f µs\n", stddev);
    printf("  Max Search Time     : %8.2f µs\n", max_t);
    printf("  Disk Usage          : %8.2f MB\n", (double)get_file_size(path) / 1e6);
    printf("  CPU Usage           : %8.2f %%\n", cpu);
    printf("  RAM Usage           : %8ld MB\n", ram_after);
}

int main() {
    freopen("../Results/results.txt", "w", stdout);
    /* --- Yollar --- */
    const char *input1      = "../RawData/Pwdb_top-10000000.txt";
    const char *input2      = "../RawData/xato-net-10-million-passwords-1000000.txt";
    const char *processed   = "../Processed/passwords_clean.txt";
    const char *raw_store   = "../Storage/Raw/passwords.txt";
    const char *hash_store  = "../Storage/Hashed/passwords_md5.txt";
    const char *comp_store  = "../Storage/Compressed/passwords.gz";

    /* --- 1. On isleme --- */
    printf("=== PREPROCESSING ===\n");
    preprocess(input1, processed, "w");
    preprocess(input2, processed, "a");

    /* --- 2. Depolama --- */
    printf("\n=== STORAGE ===\n");
    store_raw(processed, raw_store);
    store_hashed(processed, hash_store);
    store_compressed(processed, comp_store);

    /* --- 3. Rastgele 100 parola sec --- */
    static char passwords[SEARCH_COUNT][MAX_LINE];
    pick_random_passwords(processed, passwords, SEARCH_COUNT);

    /* --- 4. Performans olcumu --- */
    printf("\n=== PERFORMANCE (based on %d random searches) ===\n", SEARCH_COUNT);
    benchmark("Raw",        search_raw,        raw_store,  passwords, SEARCH_COUNT);
    benchmark("Hashed",     search_hashed,     hash_store, passwords, SEARCH_COUNT);
    benchmark("Compressed", search_compressed, comp_store, passwords, SEARCH_COUNT);

    /* --- 5. Sikistirma metrikleri --- */
    printf("\n=== COMPRESSION ===\n");
    print_compression_ratio(processed, comp_store);
    printf("Decompress Time: %.2f µs\n", decompress_time(comp_store));

    return 0;
}