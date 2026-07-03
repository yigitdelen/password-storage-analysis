#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define HASH_TABLE_SIZE 2000003

static int ht[HASH_TABLE_SIZE];

static unsigned long djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

void preprocess(const char *input_path, const char *output_path, const char *mode) {
    FILE *in  = fopen(input_path,  "r");
    FILE *out = fopen(output_path, mode);
    if (!in || !out) { perror("preprocess fopen"); return; }

    char line[MAX_LINE];
    long total = 0, written = 0;

    while (fgets(line, sizeof(line), in)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strlen(line) == 0) continue;
        total++;

        unsigned long h = djb2(line);
        if (ht[h] == 1) continue;
        ht[h] = 1;

        fprintf(out, "%s\n", line);
        written++;
    }

    fclose(in);
    fclose(out);
    printf("[preprocess] %s -> Total: %ld | Written: %ld\n", input_path, total, written);
}