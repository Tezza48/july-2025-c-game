#pragma once
#include "tzl.h"

typedef struct global_matrix_block
{
    mat4x4 model_mat;
    mat4x4 view_mat;
    mat4x4 proj_mat;
} global_matrix_block;

typedef struct _shader_t *shader;

void shader_update_resources(shader shader, global_matrix_block matrices);

void shader_use(shader mat, global_matrix_block matrices);
tzl_bool shader_load_src(const char *vert_src, const char *frag_src, shader *out_shader);
void shader_free(shader shader);

#include "asset.h"
bool assets_load_shader(assets assets, const char *name, const char *vert_src, const char *frag_src, shader *out_shader);
static inline shader assets_get_shader(assets assets, const char *name)
{
    return (shader)assets_get(assets, name);
}