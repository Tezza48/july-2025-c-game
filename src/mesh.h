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

typedef size mesh_id;
typedef struct _mesh_t *mesh;
typedef struct _mesh_t *mesh_array;

typedef struct mesh_storage
{
    mesh_array data;
    mesh_id *free_ids;
} mesh_storage;

mesh_storage mesh_storage_init();
void mesh_storage_cleanup(mesh_storage *storage);

mesh_id mesh_create(mesh_storage *storage, mesh_src_data asset);
mesh_id mesh_create_primitive_quad_y(mesh_storage *storage, f32 radius, vec4 color);

bool mesh_draw(mesh_storage *storage, vertex_layout layout, mesh_id *meshes, size num_meshes);

void mesh_cleanup(mesh_storage *storage, mesh_id mesh);
