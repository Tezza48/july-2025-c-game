#include "mesh.h"
#include "vendor/stb_ds.h"

GLuint vertex_layout_create()
{
    GLuint layout;
    glCreateVertexArrays(1, &layout);

    glEnableVertexArrayAttrib(layout, 0);
    glEnableVertexArrayAttrib(layout, 1);
    glEnableVertexArrayAttrib(layout, 2);
    // ...

    glVertexArrayAttribFormat(layout, 0, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, pos));
    glVertexArrayAttribFormat(layout, 1, 4, GL_FLOAT, GL_TRUE, offsetof(vertex, col));
    glVertexArrayAttribFormat(layout, 2, 2, GL_FLOAT, GL_TRUE, offsetof(vertex, uv));
    // ...

    glVertexArrayAttribBinding(layout, 0, 0);
    glVertexArrayAttribBinding(layout, 1, 0);
    glVertexArrayAttribBinding(layout, 2, 0);
    // ...

    return layout;
}

void vertex_layout_free(GLuint layout)
{
    glDeleteVertexArrays(1, &layout);
}

mesh mesh_create(mesh_src_data asset)
{
    mesh m = {0};

    glCreateBuffers(1, &m.vbuffer);
    glCreateBuffers(1, &m.ibuffer);

    m.num_vertices = arrlen(asset.vertices);
    m.num_indices = arrlen(asset.indices);

    glNamedBufferData(m.vbuffer, m.num_vertices * sizeof(vertex), asset.vertices, GL_STATIC_DRAW);
    glNamedBufferData(m.ibuffer, m.num_indices * sizeof(u32), asset.indices, GL_STATIC_DRAW);

    return m;
}

mesh _mesh_create_quad(vec3 corners[4], vec4 color)
{
    mesh_src_data data = {0};
    arrput(data.vertices, ((vertex){.pos = {corners[0][0], corners[0][1], corners[0][2]}, .col = {color[0], color[1], color[2], color[3]}, .uv = {0.0f, 0.0f}}));
    arrput(data.vertices, ((vertex){.pos = {corners[1][0], corners[1][1], corners[1][2]}, .col = {color[0], color[1], color[2], color[3]}, .uv = {0.0f, 1.0f}}));
    arrput(data.vertices, ((vertex){.pos = {corners[2][0], corners[2][1], corners[2][2]}, .col = {color[0], color[1], color[2], color[3]}, .uv = {1.0f, 1.0f}}));
    arrput(data.vertices, ((vertex){.pos = {corners[3][0], corners[3][1], corners[3][2]}, .col = {color[0], color[1], color[2], color[3]}, .uv = {1.0f, 0.0f}}));

    arrput(data.indices, 0);
    arrput(data.indices, 1);
    arrput(data.indices, 2);
    arrput(data.indices, 0);
    arrput(data.indices, 2);
    arrput(data.indices, 3);

    return mesh_create(data);
}

mesh mesh_create_primitive_quad_x(f32 radius, vec4 color)
{
    return _mesh_create_quad(
        (vec3[4]){
            {0.0f, -radius, -radius},
            {0.0f, radius, -radius},
            {0.0f, radius, radius},
            {0.0f, -radius, radius},
        },
        color);
}
mesh mesh_create_primitive_quad_y(f32 radius, vec4 color)
{
    return _mesh_create_quad(
        (vec3[4]){
            {-radius, 0.0f, -radius},
            {-radius, 0.0f, radius},
            {radius, 0.0f, radius},
            {radius, 0.0f, -radius},
        },
        color);
}
mesh mesh_create_primitive_quad_z(f32 radius, vec4 color)
{
    return _mesh_create_quad(
        (vec3[4]){
            {-radius, -radius, 0.0f},
            {-radius, radius, 0.0f},
            {radius, radius, 0.0f},
            {radius, -radius, 0.0f},
        },
        color);
}

mesh mesh_create_sprite(f32 width, f32 height, vec4 color) {
    return _mesh_create_quad(
        (vec3[4]){
            {0, -height, 0.0f},
            {0, 0, 0.0f},
            {width, 0, 0.0f},
            {width, -height, 0.0f},
        },
        color);
}

/// @brief
/// @param layout A vertex layout which is already bound, to which we will bind the vbuffer and ibuffer
/// @param m The mesh which has the vertex buffers
void mesh_bind(GLuint layout, mesh *m)
{
    glVertexArrayVertexBuffer(layout, 0, m->vbuffer, 0, sizeof(vertex));
    glVertexArrayElementBuffer(layout, m->ibuffer);
}

void mesh_delete(mesh m)
{
    glDeleteBuffers(1, &m.vbuffer);
    glDeleteBuffers(1, &m.ibuffer);
}