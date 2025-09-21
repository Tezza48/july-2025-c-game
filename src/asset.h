#pragma once

#include "tzl.h"

typedef struct assets *assets;

tzl_bool assets_create_alloc(assets *out_assets);

void assets_add(assets assets, const char *name, void *data);

#define assets_get(a, type, name) ((type)_assets_get((a), (name)))
void *_assets_get(assets assets, const char *name);

void assets_free(assets assets);