#include "phf.h"
#include <stdio.h>
#include <string.h>

int main() {
    libphf_t* phf = libphf_open("example.phf",0);
    if (!phf) {
        fprintf(stderr, "fail to load phf\n");
        return 1;
    }

    const char* key = "https://example.com/tile/0/0/0";
    libphf_index_t idx = libphf_get(phf, (const uint8_t*)key, strlen(key));
    printf("Index: %lld\n", (long long)idx);

    libphf_close(phf);
    return 0;
}
