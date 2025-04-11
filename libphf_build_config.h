#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int threads;
    const char* tmp_dir;
} libphf_build_config_t;

int libphf_build_from_text(const char* input_txt_path,
                           const char* output_prefix,
                           const libphf_build_config_t* config);

#ifdef __cplusplus
}
#endif
