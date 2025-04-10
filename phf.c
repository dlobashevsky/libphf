#include "phf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

struct libphf_s {
    void* data;
    size_t size;
    // TODO: добавить поля для структуры BBHash
};

libphf_t* libphf_open(const char* filepath) {
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) return NULL;

    off_t len = lseek(fd, 0, SEEK_END);
    void* map = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
    close(fd);
    if (map == MAP_FAILED) return NULL;

    libphf_t* phf = malloc(sizeof(libphf_t));
    if (!phf) {
        munmap(map, len);
        return NULL;
    }

    phf->data = map;
    phf->size = len;

    return phf;
}

void libphf_close(libphf_t* phf) {
    if (phf) {
        munmap(phf->data, phf->size);
        free(phf);
    }
}

libphf_index_t libphf_get(const libphf_t* phf, const uint8_t* key, size_t size) {
    (void)phf;
    (void)key;
    (void)size;
    // TODO: реализация BBHash lookup
    return LIBPHF_INDEX_NOT_FOUND;
}
