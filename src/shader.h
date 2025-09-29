#pragma once
#include "tzl.h"

typedef struct global_matrix_block
{
    mat4x4 model_mat;
    mat4x4 view_mat;
    mat4x4 proj_mat;
} global_matrix_block;

typedef size shader_id;
typedef struct _shader_t *shader_array;
typedef struct _shader_t *shader_p;

typedef struct shader_storage
{
    /// @brief Dynamic array (stb_ds) of shaders
    shader_array data;
} shader_storage;

shader_storage shader_storage_init();
void shader_storage_cleanup(shader_storage *storage);
shader_id shader_load_src(shader_storage *storage, const char *vert_src, const char *frag_src);

void shader_update_resources(global_matrix_block matrices);
void shader_use(shader_storage *storage, shader_id shader, global_matrix_block matrices);
