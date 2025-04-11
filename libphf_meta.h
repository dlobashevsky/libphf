#pragma once
#include <stddef.h>

typedef struct libphf_meta_t libphf_meta_t;

int libphf_meta_load(const char* filename, libphf_meta_t* meta);
const char* libphf_meta_get(const libphf_meta_t* meta, const char* key);
