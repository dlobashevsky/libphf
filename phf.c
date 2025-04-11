#include "phf.h"
#include "libphf_header.h"
#include "libphf_internal.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#define LIBPHF_HASH_FUNC(key, len, seed) ((uint64_t)XXH3_64bits_withSeed((key), (len), (seed)))

#include "xxhash.h"

struct libphf_t {
    const uint64_t* bitvector;
    uint64_t bitvector_bits;
    uint64_t n_keys;
    uint64_t seed;
    size_t size;
    void* mmap_data;
};

libphf_t* libphf_open(const char* filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) return NULL;

    off_t len = lseek(fd, 0, SEEK_END);
    void* map = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (map == MAP_FAILED) return NULL;

    if ((size_t)len < sizeof(libphf_header_t)) {
        munmap(map, len);
        return NULL;
    }

    libphf_t* phf = malloc(sizeof(libphf_t));
    if (!phf) {
        munmap(map, len);
        return NULL;
    }

    const uint64_t* hdr = (const uint64_t*)map;
    phf->bitvector_bits = hdr[0];
    phf->n_keys = hdr[1];
    phf->seed = hdr[2];
    phf->bitvector = hdr + 3;

    phf->mmap_data = map;
    phf->size = len;

    return phf;
}

void libphf_close(libphf_t* phf) {
    if (phf) {
        munmap(phf->mmap_data, phf->size);
        free(phf);
    }
}

libphf_index_t libphf_get(const libphf_t* ctx, const uint8_t* key, size_t len) {
    if (!ctx || !key || !ctx->bitvector || ctx->bitvector_bits == 0) return LIBPHF_INDEX_NOT_FOUND;

    uint64_t hash = LIBPHF_HASH_FUNC(key, len, ctx->seed);
    uint64_t idx = hash % ctx->bitvector_bits;

    size_t word_idx = idx / 64;
    size_t bit_idx = idx % 64;

    if (word_idx >= (ctx->bitvector_bits + 63) / 64) return LIBPHF_INDEX_NOT_FOUND;

    uint64_t word = ctx->bitvector[word_idx];
    if ((word & ((uint64_t)1 << bit_idx)) == 0)
        return LIBPHF_INDEX_NOT_FOUND;

    return (libphf_index_t)idx;
}
