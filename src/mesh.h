#pragma once

#include "tzl.h"
#include <glad/glad.h>

GLuint vertex_layout_create();
void vertex_layout_free(GLuint layout);

typedef struct vertex
{
    vec3 pos;
    vec4 col;
    vec2 uv;
} vertex;

typedef struct mesh_src_data
{
    vertex *vertices;
    u32 *indices;
} mesh_src_data;

typedef struct mesh
{
    size num_vertices;
    size num_indices;
    GLuint vbuffer, ibuffer;
} mesh;

mesh mesh_create(mesh_src_data asset);
mesh mesh_create_primitive_quad_x(f32 radius, vec4 color);
mesh mesh_create_primitive_quad_y(f32 radius, vec4 color);
mesh mesh_create_primitive_quad_z(f32 radius, vec4 color);
mesh mesh_create_sprite(f32 width, f32 height, vec4 color);
void mesh_bind(GLuint layout, mesh *m);
void mesh_delete(mesh m);
