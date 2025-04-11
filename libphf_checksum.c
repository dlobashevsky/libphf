#include "libphf_checksum.h"
#include "xxhash.h"
#include <string.h>

void libphf_compute_checksum(const void* data, size_t len, uint8_t out[16]) {
    XXH128_hash_t hash = XXH3_128bits(data, len);
    memcpy(out, &hash, 16);
}
