#include "libphf_build_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#define IDX_ENTRY_SIZE sizeof(struct phf_idx_record)

#pragma pack(push, 1)
struct phf_idx_record {
    uint64_t offset;
    uint32_t length;
};
#pragma pack(pop)

static off_t estimate_file_size(const char* path, size_t* n_lines, size_t* total_bytes) {
    FILE* f = fopen(path, "r");
    if (!f) return -1;

    size_t lines = 0;
    size_t total = 0;
    char* line = NULL;
    size_t len = 0;

    while (getline(&line, &len, f) != -1) {
        total += strlen(line) + 1;
        ++lines;
    }

    free(line);
    fclose(f);
    if (n_lines) *n_lines = lines;
    if (total_bytes) *total_bytes = total;
    return 0;
}

int libphf_build_from_text(const char* input_txt_path,
                           const char* output_prefix,
                           const libphf_build_config_t* config) {
    if (!input_txt_path || !output_prefix || !config || !config->tmp_dir)
        return -1;

    size_t n_lines = 0;
    size_t str_bytes = 0;
    if (estimate_file_size(input_txt_path, &n_lines, &str_bytes) != 0)
        return -2;

    char str_path[1024], idx_path[1024];
    snprintf(str_path, sizeof(str_path), "%s.str", output_prefix);
    snprintf(idx_path, sizeof(idx_path), "%s.idx", output_prefix);

    int fd_str = open(str_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd_str < 0) return -3;
    if (posix_fallocate(fd_str, 0, str_bytes) != 0) {
        close(fd_str);
        return -4;
    }

    int fd_idx = open(idx_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd_idx < 0) {
        close(fd_str);
        return -5;
    }
    if (posix_fallocate(fd_idx, 0, n_lines * IDX_ENTRY_SIZE) != 0) {
        close(fd_str);
        close(fd_idx);
        return -6;
    }

    void* map_str = mmap(NULL, str_bytes, PROT_WRITE, MAP_SHARED, fd_str, 0);
    void* map_idx = mmap(NULL, n_lines * IDX_ENTRY_SIZE, PROT_WRITE, MAP_SHARED, fd_idx, 0);
    close(fd_str);
    close(fd_idx);
    if (map_str == MAP_FAILED || map_idx == MAP_FAILED) {
        if (map_str != MAP_FAILED) munmap(map_str, str_bytes);
        if (map_idx != MAP_FAILED) munmap(map_idx, n_lines * IDX_ENTRY_SIZE);
        return -7;
    }

    FILE* f = fopen(input_txt_path, "r");
    if (!f) return -8;

    char* line = NULL;
    size_t len = 0;
    size_t offset = 0;
    size_t index = 0;

    while (getline(&line, &len, f) != -1 && index < n_lines) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') line[--l] = 0;

        if (offset + l + 1 > str_bytes) {
            munmap(map_str, str_bytes);
            munmap(map_idx, n_lines * IDX_ENTRY_SIZE);
            fclose(f);
            free(line);
            return -9;
        }

        memcpy((char*)map_str + offset, line, l);
        ((char*)map_str)[offset + l] = '\0';

        struct phf_idx_record* rec = ((struct phf_idx_record*)map_idx) + index;
        rec->offset = offset;
        rec->length = (uint32_t)l;

        offset += l + 1;
        ++index;
    }

    free(line);
    fclose(f);
    munmap(map_str, str_bytes);
    munmap(map_idx, n_lines * IDX_ENTRY_SIZE);

    return 0;
}
