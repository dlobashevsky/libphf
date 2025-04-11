#pragma once
#include "libphf_header.h"
#include <stdint.h>
#include <stddef.h>

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
} libphf_t;

#include "libphf_meta.h"

libphf_t* libphf_open_from_mapped(const libphf_meta_t* meta,
                                  const libphf_header_t* hdr_bbh, const void* data_bbh,
                                  const libphf_header_t* hdr_idx, const void* data_idx,
                                  const libphf_header_t* hdr_str, const void* data_str,
                                  uint32_t flags);
