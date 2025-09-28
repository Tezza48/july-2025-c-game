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

bool mesh_primitive_quad_y(float radius, vec4 color, mesh *out_mesh);

bool mesh_draw(vertex_layout layout, mesh *meshes, size num_meshes);
void mesh_free(mesh mesh);
