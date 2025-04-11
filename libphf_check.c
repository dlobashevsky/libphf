#include "libphf_meta.h"
#include "libphf_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define LIBPHF_FLAG_NONE            0x0000
#define LIBPHF_FLAG_SKIP_STRINGS    0x0001
#define LIBPHF_FLAG_SKIP_CHECKSUM   0x0002
#define LIBPHF_FLAG_STRICT_UUID     0x0004

// stub for xxHash128, real implementation should be linked
void libphf_compute_checksum(const void* data, size_t len, uint8_t out[16]) {
    for (int i = 0; i < 16; ++i) out[i] = (uint8_t)(i + len % 255); // placeholder
}

int libphf_check_file(const char* path, const libphf_header_t* expected, uint32_t flags, const char* label) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return -1;

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        return -2;
    }

    void* data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (data == MAP_FAILED) return -3;

    const libphf_header_t* hdr = (const libphf_header_t*)data;
    if (memcmp(hdr->magic, "PHF", 3) != 0) {
        munmap(data, st.st_size);
        return -4;
    }

    if (expected && libphf_check_headers(expected, hdr) != 0) {
        munmap(data, st.st_size);
        return -5;
    }

    if (!(flags & LIBPHF_FLAG_SKIP_CHECKSUM)) {
        uint8_t hash[16];
        libphf_compute_checksum((const uint8_t*)data + sizeof(libphf_header_t),
                                st.st_size - sizeof(libphf_header_t), hash);
        if (memcmp(hash, hdr->checksum, 16) != 0) {
            munmap(data, st.st_size);
            return -6;
        }
    }

    munmap(data, st.st_size);
    return 0;
}

int libphf_check(const char* metafile, uint32_t flags) {
    libphf_meta_t meta;
    if (libphf_meta_load(metafile, &meta) != 0) return -10;

    const char* bbh = libphf_meta_get(&meta, "bitvector_file");
    const char* idx = libphf_meta_get(&meta, "index_file");
    const char* str = libphf_meta_get(&meta, "keys_file");

    if (!bbh) return -11;

    libphf_header_t hdr_main;
    int r = libphf_check_file(bbh, NULL, flags, "bbh");
    if (r != 0) return -20 + r;

    // reread header for comparison
    {
        int fd = open(bbh, O_RDONLY);
        if (fd < 0) return -21;
        read(fd, &hdr_main, sizeof(hdr_main));
        close(fd);
    }

    if (!(flags & LIBPHF_FLAG_SKIP_STRINGS)) {
        if (idx) {
            r = libphf_check_file(idx, &hdr_main, flags, "idx");
            if (r != 0) return -30 + r;
        }
        if (str) {
            r = libphf_check_file(str, &hdr_main, flags, "str");
            if (r != 0) return -40 + r;
        }
    }

    return 0;
}
