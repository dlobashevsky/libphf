#ifndef _PHF_H
#include <stddef.h>
#include <stdint.h>

#define LIBPHF_INDEX_NOT_FOUND (-1LL)
typedef int64_t libphf_index_t;

typedef struct libphf_t libphf_t;


//! sringfile safe to zero, in this case string comparition is not performed
libphf_t* libphf_open(const char* filename);

void libphf_close(libphf_t*);

libphf_index_t libphf_get(const libphf_t*, const uint8_t* key, size_t size);

#define _PHF_H
#endif
