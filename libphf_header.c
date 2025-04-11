#include "libphf_header.h"
#include <string.h>

int libphf_check_headers(const libphf_header_t* base, const libphf_header_t* other) {
    if (!base || !other) return -1;

    if (memcmp(base->uuid, other->uuid, LIBPHF_UUID_LEN) != 0)
        return -2;

    if (base->n_keys != other->n_keys)
        return -3;

    // Дополнительные проверки можно добавить по мере надобности

    return 0; // Всё ок
}
