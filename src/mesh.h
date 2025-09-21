#pragma once

#include "tzl.h"

typedef struct model *model;

typedef struct shader *shader;

void shader_use(shader shader);
tzl_bool shader_load_src(const char *vert_src, const char *frag_src, shader *out_shader);
void shader_free(shader shader);