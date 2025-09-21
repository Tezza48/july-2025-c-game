#pragma once

#include "tzl.h"

typedef void (*assets_destructor)(void *asset);

typedef struct assets *assets;

tzl_bool assets_create_alloc(assets *out_assets);

/*
 This takes ownership of the asset if you provide a destructor
*/
void assets_add(assets assets, const char *name, void *data, assets_destructor dtor);

void *assets_get(assets assets, const char *name);

void assets_free(assets assets);
