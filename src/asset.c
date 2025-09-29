#include "asset.h"

#include "../vendor/stb_ds.h"

typedef struct _entry
{
    const char *key;
    void *value;
    assets_destructor dtor;
} _entry;

typedef struct assets
{
    _entry *data;
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

void assets_add(assets assets, const char *name, void *data, assets_destructor dtor)
{
    // char *arr = assets->data;
    // arrput(arr, data);
    // assets->data = arr;
    _entry entry;

    entry = (_entry){
        .key = name,
        .value = data,
        .dtor = dtor,
    };
    hmputs(assets->data, entry);
}

void *assets_get(assets assets, const char *name)
{
    return hmget(assets->data, name);
}

void assets_free(assets assets)
{
    if (assets->data != NULL)
    {
        for (int i = 0; i < hmlen(assets->data); i++)
        {
            if (assets->data[i].dtor != NULL)
            {
                assets->data[i].dtor(assets->data[i].value);
            }
        }
        hmfree(assets->data);
    }

    free(assets);
}
