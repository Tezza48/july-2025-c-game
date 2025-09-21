#pragma once

#include "tzl.h"

typedef struct model *model;

typedef struct shader *shader;

void shader_use(shader shader);
tzl_bool shader_load_src(const char *vert_src, const char *frag_src, shader *out_shader);
void shader_free(shader shader);

#include "asset.h"
bool assets_load_shader(assets assets, const char *name, const char *vert_src, const char *frag_src, shader *out_shader);
static inline shader assets_get_shader(assets assets, const char *name)
{
    return (shader)assets_get(assets, name);
}