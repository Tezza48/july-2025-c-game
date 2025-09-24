#pragma once

#include "tzl.h"

typedef struct vertex_layout *vertex_layout;

bool vertex_layout_create(vertex_layout *out_layout);

typedef struct vertex
{
    vec3 pos;
    vec4 col;
} vertex;

typedef struct mesh_src_data
{
    vertex *vertices;
    u32 *indices;
} mesh_src_data;

typedef struct mesh *mesh;

bool mesh_create(mesh_src_data asset, mesh *out_mesh);
bool mesh_draw(vertex_layout layout, mesh *meshes);
void mesh_free(mesh mesh);

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