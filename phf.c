#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "phf.h"

#ifndef LIBPHF_HASH_FUNC
#define LIBPHF_HASH_FUNC libphf_dummy_hash
#endif

static uint64_t libphf_dummy_hash(const uint8_t* data, size_t len, uint64_t seed) {
    // Временная заглушка: простой хеш (сумма + seed)
    uint64_t h = seed;
    for (size_t i = 0; i < len; ++i)
        h = h * 31 + data[i];
    return h;
}

struct libphf_s {
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

    if ((size_t)len < sizeof(libphf_t)) {
        munmap(map, len);
        return NULL;
    }

    libphf_t* phf = malloc(sizeof(libphf_t));
    if (!phf) {
        munmap(map, len);
        return NULL;
    }

    madvise(map, len, MADV_RANDOM);

    phf->mmap_data = map;
    phf->size = len;

    // Читаем заголовок (заглушка)
    const uint64_t* hdr = (const uint64_t*)map;
    phf->bitvector_bits = hdr[0];
    phf->n_keys = hdr[1];
    phf->seed = hdr[2];
    phf->bitvector = hdr + 3;

    return phf;
}

void libphf_close(libphf_t* phf) {
    if (phf) {
        munmap(phf->mmap_data, phf->size);
        free(phf);
    }
}

libphf_index_t libphf_get(const libphf_t* phf, const uint8_t* key, size_t size) {
    uint64_t h = LIBPHF_HASH_FUNC(key, size, phf->seed);
    uint64_t idx = h % phf->bitvector_bits;

    uint64_t word = phf->bitvector[idx / 64];
    uint64_t bit = (word >> (idx % 64)) & 1;

    if (bit == 0)
        return LIBPHF_INDEX_NOT_FOUND;

    return (libphf_index_t)idx;
}
