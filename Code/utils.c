#include "utils.h"
#include <windows.h>
#include <psapi.h>

double get_time_us() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1e6 + ts.tv_nsec / 1e3;
}

long get_file_size(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) return st.st_size;
    return -1;
}

void log_result(const char *label, double avg_us, double max_us, long size_bytes) {
    printf("%-12s | Avg: %8.2f µs | Max: %8.2f µs | Size: %ld bytes\n",
           label, avg_us, max_us, size_bytes);
}

long get_ram_usage_mb() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)))
        return pmc.WorkingSetSize / (1024 * 1024);
    return -1;
}