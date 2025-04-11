#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void libphf_compute_checksum(const void* data, size_t len, uint8_t out[16]);

#ifdef __cplusplus
}
#endif
