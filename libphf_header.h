#pragma once
#include <stdint.h>
#include <stddef.h>


#define LIBPHF_MAGIC "\x0d\xec\xca"
#define LIBPHF_MAGIC_LEN 3
#define LIBPHF_FORMAT_VERSION 1
#define LIBPHF_UUID_LEN 16
#define LIBPHF_CHECKSUM_LEN 16

typedef struct __attribute__((packed)) {
    uint8_t  magic[LIBPHF_MAGIC_LEN];
    uint8_t  format_version;
    uint8_t  checksum[LIBPHF_CHECKSUM_LEN];
    uint8_t  uuid[LIBPHF_UUID_LEN];
    uint64_t bitvector_bits;
    uint64_t n_keys;
    uint64_t seed;
    uint64_t size;
} libphf_header_t;

typedef struct {
    uint8_t uuid[LIBPHF_UUID_LEN];
    uint64_t n_keys;
    uint64_t seed;
    int has_keys;
    int has_idx;
    int has_val;

    const uint64_t* bitvector;
    size_t bitvector_words;

    const uint64_t* idx_array;
    const char* str_data;
    size_t str_data_size;
} libphf_internal_t;

