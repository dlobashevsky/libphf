#pragma once
#include <stddef.h>

typedef struct libphf_meta_t libphf_meta_t;

//! return 0 if error
const libphf_meta_t* libphf_meta_load(const char* filename);
const char* libphf_meta_get(const libphf_meta_t* meta, const char* key);
