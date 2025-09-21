#include "asset.h"

#include "../vendor/stb_ds.h"

typedef struct assets
{
    struct
    {
        const char *key;
        void *value;
    } *data;
} *assets;

tzl_bool assets_create_alloc(assets *out_assets)
{
    assets out = (assets)calloc(1, sizeof(struct assets));
    if (!out)
    {
        TZL_LOG_ERROR("Couldn't allocate space for assets*");
        return false;
    }

    *out_assets = out;
    return true;
}

void assets_add(assets assets, const char *name, void *data)
{
    // char *arr = assets->data;
    // arrput(arr, data);
    // assets->data = arr;

    hmput(assets->data, name, data);
}

void *_assets_get(assets assets, const char *name)
{
    return hmget(assets->data, name);
}

void assets_free(assets assets)
{
    hmfree(assets->data);
    free(assets);
}
