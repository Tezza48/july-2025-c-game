#pragma once
#include "tzl.h"

#include <glad/glad.h>

typedef struct global_matrix_block
{
    mat4x4 model_mat;
    mat4x4 view_mat;
    mat4x4 proj_mat;
} global_matrix_block;

/// @brief Creates a GL program from the provided shader sources.
GLuint shader_load_src(const char *vert_src, const char *frag_src);

void shader_standard_update_resources(global_matrix_block matrices);
void shader_delete(GLuint program);
