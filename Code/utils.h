#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

double get_time_us();
long   get_file_size(const char* path);
void   log_result(const char* label, double avg_us, double max_us, long size_bytes);
double get_cpu_usage();
long get_ram_usage_mb();

#endif