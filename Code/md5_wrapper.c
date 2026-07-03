#include "md5.h"
#include <stdio.h>
#include <string.h>

void md5_string(const char *input, char output[33]) {
    BYTE digest[16];
    MD5_CTX ctx;
    md5_init(&ctx);
    md5_update(&ctx, (BYTE*)input, strlen(input));
    md5_final(&ctx, digest);
    for (int i = 0; i < 16; i++)
        sprintf(output + i*2, "%02x", digest[i]);
    output[32] = 0;
}