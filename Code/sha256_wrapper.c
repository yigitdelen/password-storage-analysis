#include "sha256.h"
#include <stdio.h>
#include <string.h>

void sha256_string(const char *input, char output[65]) {
    BYTE digest[32];
    SHA256_CTX ctx;
    sha256_init(&ctx);
    sha256_update(&ctx, (BYTE*)input, strlen(input));
    sha256_final(&ctx, digest);
    for (int i = 0; i < 32; i++)
        sprintf(output + i*2, "%02x", digest[i]);
    output[64] = 0;
}