#include "libphf_meta.h"
#include "libphf_header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#define LIBPHF_FLAG_NONE            0x0000
#define LIBPHF_FLAG_SKIP_STRINGS    0x0001
#define LIBPHF_FLAG_SKIP_CHECKSUM   0x0002

extern void libphf_compute_checksum(const void* data, size_t len, uint8_t out[16]);

int libphf_check(const char* metafile, uint32_t flags) {
    const libphf_meta_t* meta=libphf_meta_load(metafile);
    if (!meta) return -1;

    const char* bbh = libphf_meta_get(meta, "bitvector_file");
    const char* idx = libphf_meta_get(meta, "index_file");
    const char* str = libphf_meta_get(meta, "keys_file");

    if (!bbh) return -2;

    const char* files_to_check[3] = {bbh, idx, str};
    const char* labels[3] = {"bbh", "idx", "str"};
    int check_flags[3] = {1, !(flags & LIBPHF_FLAG_SKIP_STRINGS), !(flags & LIBPHF_FLAG_SKIP_STRINGS)};

    libphf_header_t ref_hdr;
    int ref_set = 0;

    for (int i = 0; i < 3; ++i) {
        if (!files_to_check[i] || !check_flags[i])
            continue;

        const char* path = files_to_check[i];
        int fd = open(path, O_RDONLY);
        if (fd < 0) return -10;

        struct stat st;
        if (fstat(fd, &st) < 0) {
            close(fd);
            return -11;
        }

        if ((size_t)st.st_size < sizeof(libphf_header_t)) {
            close(fd);
            return -12;
        }

        void* data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
        if (data == MAP_FAILED) return -13;

        libphf_header_t* hdr = (libphf_header_t*)data;

        if (memcmp(hdr->magic, "PHF", 3) != 0) {
            munmap(data, st.st_size);
            return -14;
        }

        if (!ref_set) {
            memcpy(&ref_hdr, hdr, sizeof(libphf_header_t));
            ref_set = 1;
        } else {
            if (memcmp(hdr->uuid, ref_hdr.uuid, sizeof(ref_hdr.uuid)) != 0) {
                munmap(data, st.st_size);
                return -15;
            }
            if (hdr->n_keys != ref_hdr.n_keys) {
                munmap(data, st.st_size);
                return -16;
            }
        }

        if (!(flags & LIBPHF_FLAG_SKIP_CHECKSUM)) {
            uint8_t hash[16];
            libphf_compute_checksum((uint8_t*)data + sizeof(libphf_header_t),
                                    st.st_size - sizeof(libphf_header_t), hash);
            if (memcmp(hash, hdr->checksum, sizeof(hdr->checksum)) != 0) {
                munmap(data, st.st_size);
                return -17;
            }
        }

        munmap(data, st.st_size);
    }

    return 0;
}
