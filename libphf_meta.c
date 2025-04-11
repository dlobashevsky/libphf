#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_META_ENTRIES 64
#define MAX_KEY_LEN 64
#define MAX_VALUE_LEN 256

typedef struct {
    char* key;
    char* value;
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

const libphf_meta_t* libphf_meta_init(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) return 0;

    libphf_meta_t* meta=calloc(1,sizeof(libphf_meta_t));
    size_t z=0;
    char *line=0;
    size_t count = 0;

    while(getline(&line,&z,fp)>=0)
    {
      if(!*line || *line=='#' || isspace(*line))
        continue;
      char* trimmed = trim(line);
      char* eq = strchr(trimmed, '=');
      if (!eq || eq == trimmed) continue;

      *eq = '\0';
      char* key = trim(trimmed);
      char* value = trim(eq + 1);
      if (count >= MAX_META_ENTRIES) break;
      meta->entries[count].key=strdup(key);
      meta->entries[count].value=strdup(value);

      count++;
    }

    fclose(fp);
    meta->count = count;
    free(line);
    return meta;
}

void libphf_meta_free(const libphf_meta_t* meta)
{
  libphf_meta_t* m=(void*)meta;
  if(!m)  return;
  while(m->count--)
  {
    free(meta->entries[m->count].key);
    free(meta->entries[m->count].value);
  }
  free(m);
}

const char* libphf_meta_get(const libphf_meta_t* meta, const char* key) {
    for (size_t i = 0; i < meta->count; ++i) {
        if (strcmp(meta->entries[i].key, key) == 0)
            return meta->entries[i].value;
    }
    return NULL;
}
