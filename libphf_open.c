#include "libphf_internal.h"
#include "libphf_header.h"
#include "libphf_meta.h"
#include <string.h>
#include <stdlib.h>

libphf_t* libphf_open_from_mapped(const libphf_meta_t* meta,
                                  const libphf_header_t* hdr_bbh, const void* data_bbh,
                                  const libphf_header_t* hdr_idx, const void* data_idx,
                                  const libphf_header_t* hdr_str, const void* data_str,
                                  uint32_t flags)
{
    if (!meta || !hdr_bbh || !data_bbh) return NULL;

    libphf_t* ctx = calloc(1, sizeof(libphf_t));
    if (!ctx) return NULL;

    ctx->n_keys = hdr_bbh->n_keys;
    ctx->seed = hdr_bbh->seed;
    ctx->bitvector_bits = hdr_bbh->bitvector_bits;
    ctx->bitvector = (const uint64_t*)((const uint8_t*)data_bbh + sizeof(libphf_header_t));
    ctx->bitvector_words = (hdr_bbh->bitvector_bits + 63) / 64;

    if (hdr_idx && data_idx) {
        ctx->idx_array = (const uint64_t*)((const uint8_t*)data_idx + sizeof(libphf_header_t));
        ctx->has_idx = 1;
    }

    if (hdr_str && data_str) {
        ctx->str_data = (const char*)((const uint8_t*)data_str + sizeof(libphf_header_t));
        ctx->has_keys = 1;
    }

    memcpy(ctx->uuid, hdr_bbh->uuid, sizeof(ctx->uuid));
    ctx->flags = flags;

    return ctx;
}
