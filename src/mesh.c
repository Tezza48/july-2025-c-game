#include "mesh.h"
#include <glad/glad.h>

#include "../vendor/stb_ds.h"

struct vertex_layout
{
    GLuint layout;
};

bool vertex_layout_create(vertex_layout *out_layout)
{
    vertex_layout layout = calloc(1, sizeof(*layout));
    if (!layout)
    {
        TZL_LOG_ERROR("Failed to create vertex_layout");
        return false;
    }

    glCreateVertexArrays(1, &layout->layout);

    glEnableVertexArrayAttrib(layout->layout, 0);
    glEnableVertexArrayAttrib(layout->layout, 1);
    // ...

    glVertexArrayAttribFormat(layout->layout, 0, 3, GL_FLOAT, GL_FALSE, offsetof(vertex, pos));
    glVertexArrayAttribFormat(layout->layout, 1, 4, GL_FLOAT, GL_TRUE, offsetof(vertex, col));
    // ...

    glVertexArrayAttribBinding(layout->layout, 0, 0);
    glVertexArrayAttribBinding(layout->layout, 1, 0);
    // ...

    *out_layout = layout;

    return true;
}

void vertex_layout_free(vertex_layout layout)
{
    glDeleteVertexArrays(1, &layout->layout);
    free(layout);
}

struct mesh
{
    size num_vertices;
    size num_indices;
    GLuint vbuffer, ibuffer;
};

bool mesh_create(mesh_src_data asset, mesh *out_mesh)
{
    mesh mesh = calloc(1, sizeof(*mesh));
    if (!mesh)
    {
        TZL_LOG_ERROR("Failed to create mesh");
        return false;
    }

    glCreateBuffers(1, &mesh->vbuffer);
    glCreateBuffers(1, &mesh->ibuffer);

    mesh->num_vertices = arrlen(asset.vertices);
    mesh->num_indices = arrlen(asset.indices);

    glNamedBufferData(mesh->vbuffer, mesh->num_vertices * sizeof(vertex), asset.vertices, GL_STATIC_DRAW);
    glNamedBufferData(mesh->ibuffer, mesh->num_indices * sizeof(u32), asset.indices, GL_STATIC_DRAW);

    *out_mesh = mesh;

    return true;
}

bool mesh_draw(vertex_layout layout, mesh *meshes, size num_meshes)
{
    glBindVertexArray(layout->layout);

    for (size i = 0; i < num_meshes; i++)
    {
        mesh mesh = meshes[i];
        glVertexArrayVertexBuffer(layout->layout, 0, mesh->vbuffer, 0, sizeof(vertex));
        glVertexArrayElementBuffer(layout->layout, mesh->ibuffer);

        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->num_indices, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    return true;
}

bool mesh_primitive_quad_y(float radius, vec4 color, mesh *out_mesh)
{
    mesh_src_data data = {0};
    arrput(data.vertices, ((vertex){.pos = {-radius, 0.0f, -radius}, .col = {color[0], color[1], color[2], color[3]}}));
    arrput(data.vertices, ((vertex){.pos = {-radius, 0.0f, radius}, .col = {color[0], color[1], color[2], color[3]}}));
    arrput(data.vertices, ((vertex){.pos = {radius, 0.0f, radius}, .col = {color[0], color[1], color[2], color[3]}}));
    arrput(data.vertices, ((vertex){.pos = {radius, 0.0f, -radius}, .col = {color[0], color[1], color[2], color[3]}}));

    arrput(data.indices, 0);
    arrput(data.indices, 1);
    arrput(data.indices, 2);
    arrput(data.indices, 0);
    arrput(data.indices, 2);
    arrput(data.indices, 3);

    return mesh_create(data, out_mesh);
}

void mesh_free(mesh mesh)
{
    glDeleteBuffers(1, &mesh->vbuffer);
    glDeleteBuffers(1, &mesh->ibuffer);

    free(mesh);
}
