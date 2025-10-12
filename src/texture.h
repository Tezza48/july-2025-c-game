#pragma once
#include "tzl.h"

typedef size texture_id;
typedef struct _texture_t *texture_p;
typedef struct _texture_t *texture_array;

typedef struct texture_storage
{
    texture_array data;
} texture_storage;

texture_storage texture_storage_init();
void texture_storage_cleanup(texture_storage *storage);
texture_id texture_load_file(texture_storage *storage, const char *filename);
