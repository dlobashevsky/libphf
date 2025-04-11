#include "libphf_internal.h"
#include "libphf_hash.h"
#include "phf.h"

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
