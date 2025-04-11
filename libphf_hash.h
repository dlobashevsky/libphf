#pragma once
#include <stddef.h>
#include <stdint.h>
#include "xxhash.h"

#define LIBPHF_HASH_FUNC(key, len, seed) ((uint64_t)XXH3_64bits_withSeed((key), (len), (seed)))
