#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_META_ENTRIES 64
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} libphf_meta_entry_t;

typedef struct {
    libphf_meta_entry_t entries[MAX_META_ENTRIES];
    size_t count;
} libphf_meta_t;

static char* trim(char* str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

int libphf_meta_load(const char* filename, libphf_meta_t* meta) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return -1;

    char line[512];
    size_t count = 0;

    while (fgets(line, sizeof(line), fp)) {
        char* trimmed = trim(line);
        if (trimmed[0] == '#' || trimmed[0] == '\0') continue;

        char* eq = strchr(trimmed, '=');
        if (!eq || eq == trimmed) continue;

        *eq = '\0';
        char* key = trim(trimmed);
        char* value = trim(eq + 1);

        if (count >= MAX_META_ENTRIES) break;
        strncpy(meta->entries[count].key, key, MAX_KEY_LEN - 1);
        strncpy(meta->entries[count].value, value, MAX_VALUE_LEN - 1);
        count++;
    }

    fclose(fp);
    meta->count = count;
    return 0;
}

const char* libphf_meta_get(const libphf_meta_t* meta, const char* key) {
    for (size_t i = 0; i < meta->count; ++i) {
        if (strcmp(meta->entries[i].key, key) == 0)
            return meta->entries[i].value;
    }
    return NULL;
}
