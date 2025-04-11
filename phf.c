#include "phf.h"
#include "xxhash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/stat.h>
#include <errno.h>

#define LIBPHF_HASH_FUNC(key, len, seed) ((uint64_t)XXH3_64bits_withSeed((key), (len), (seed)))
#define LIBPHF_INDEX_NOT_FOUND (-1LL)

typedef int64_t libphf_index_t;

typedef struct libphf_t {
    uint64_t n_keys;
    uint64_t seed;
    uint64_t bitvector_bits;
    size_t bitvector_words;
    const uint64_t* bitvector;

    const uint64_t* idx_array;
    const char* str_data;

    uint8_t uuid[16];
    int has_idx;
    int has_keys;
    uint32_t flags;

    void* mmap_data_bbh;
    size_t mmap_size_bbh;
    void* mmap_data_idx;
    size_t mmap_size_idx;
    void* mmap_data_str;
    size_t mmap_size_str;
} libphf_t;

static void* phf_map_file(const char* path, size_t* size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;

    struct stat st;
    if (fstat(fd, &st) != 0) {
        close(fd);
        return NULL;
    }

    void* map = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (map == MAP_FAILED) return NULL;

    if (size) *size = st.st_size;
    return map;
}

libphf_t* libphf_open_simple(const char* bbh_path, const char* idx_path, const char* str_path) {
    libphf_t* phf = calloc(1, sizeof(libphf_t));
    if (!phf) return NULL;

    size_t sz = 0;
    phf->mmap_data_bbh = phf_map_file(bbh_path, &sz);
    if (!phf->mmap_data_bbh) goto fail;
    phf->mmap_size_bbh = sz;

    const uint64_t* hdr = (const uint64_t*)phf->mmap_data_bbh;
    phf->bitvector_bits = hdr[0];
    phf->n_keys = hdr[1];
    phf->seed = hdr[2];
    phf->bitvector = hdr + 3;
    phf->bitvector_words = (phf->bitvector_bits + 63) / 64;

    if (idx_path) {
        phf->mmap_data_idx = phf_map_file(idx_path, &sz);
        if (!phf->mmap_data_idx) goto fail;
        phf->mmap_size_idx = sz;
        phf->idx_array = (const uint64_t*)phf->mmap_data_idx;
        phf->has_idx = 1;
    }

    if (str_path) {
        phf->mmap_data_str = phf_map_file(str_path, &sz);
        if (!phf->mmap_data_str) goto fail;
        phf->mmap_size_str = sz;
        phf->str_data = (const char*)phf->mmap_data_str;
        phf->has_keys = 1;
    }

    return phf;

fail:
    munmap(phf->mmap_data_bbh, phf->mmap_size_bbh);
    if (phf->mmap_data_idx) munmap(phf->mmap_data_idx, phf->mmap_size_idx);
    if (phf->mmap_data_str) munmap(phf->mmap_data_str, phf->mmap_size_str);
    free(phf);
    return NULL;
}

void libphf_close(libphf_t* phf) {
    if (!phf) return;
    munmap(phf->mmap_data_bbh, phf->mmap_size_bbh);
    if (phf->mmap_data_idx) munmap(phf->mmap_data_idx, phf->mmap_size_idx);
    if (phf->mmap_data_str) munmap(phf->mmap_data_str, phf->mmap_size_str);
    free(phf);
}

libphf_index_t libphf_get(const libphf_t* ctx, const uint8_t* key, size_t len) {
    if (!ctx || !key || !ctx->bitvector || ctx->bitvector_bits == 0) return LIBPHF_INDEX_NOT_FOUND;

    uint64_t hash = LIBPHF_HASH_FUNC(key, len, ctx->seed);
    uint64_t idx = hash % ctx->bitvector_bits;

    size_t word_idx = idx / 64;
    size_t bit_idx = idx % 64;

    if (word_idx >= ctx->bitvector_words) return LIBPHF_INDEX_NOT_FOUND;

    uint64_t word = ctx->bitvector[word_idx];
    if ((word & ((uint64_t)1 << bit_idx)) == 0)
        return LIBPHF_INDEX_NOT_FOUND;

    return (libphf_index_t)idx;
}
