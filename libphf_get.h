#pragma once
#include <stddef.h>
#include <stdint.h>

typedef int64_t libphf_index_t;
struct libphf_t;

#define LIBPHF_INDEX_NOT_FOUND (-1LL)

libphf_index_t libphf_get(const struct libphf_t* ctx, const uint8_t* key, size_t len);
