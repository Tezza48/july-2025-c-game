#include "mesh.h"
#include <glad/glad.h>

#include "../vendor/stb_ds.h"

typedef struct _mesh_t
{
    size num_vertices;
    size num_indices;
    GLuint vbuffer, ibuffer;
} _mesh_t;

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

mesh_storage mesh_storage_init()
{
    return (mesh_storage){0};
}

void mesh_storage_cleanup(mesh_storage *storage)
{
    for (size i = 0; i < arrlen(storage->data); i++)
    {
        mesh_cleanup(storage, i);
    }

    arrfree(storage->data);
    arrfree(storage->free_ids);
}

mesh_id mesh_create(mesh_storage *storage, mesh_src_data asset)
{
    _mesh_t mesh = {0};

    glCreateBuffers(1, &mesh.vbuffer);
    glCreateBuffers(1, &mesh.ibuffer);

    mesh.num_vertices = arrlen(asset.vertices);
    mesh.num_indices = arrlen(asset.indices);

    glNamedBufferData(mesh.vbuffer, mesh.num_vertices * sizeof(vertex), asset.vertices, GL_STATIC_DRAW);
    glNamedBufferData(mesh.ibuffer, mesh.num_indices * sizeof(u32), asset.indices, GL_STATIC_DRAW);

    size id;
    size free_len = arrlen(storage->free_ids);
    if (free_len)
    {
        // pop the last free_id and insert the new mesh at that position.
        id = storage->free_ids[arrlen(free_len - 1)];
        arrsetlen(storage->free_ids, free_len - 1);
        storage->data[id] = mesh;
    }
    else
    {
        // push the new mesh to the end.
        id = arrlen(storage->data);
        arrput(storage->data, mesh);
    }

    return id;
}

bool mesh_draw(mesh_storage *storage, vertex_layout layout, mesh_id *mesh_ids, size num_meshes)
{
    glBindVertexArray(layout->layout);

    for (size i = 0; i < num_meshes; i++)
    {
        _mesh_t *mesh = &storage->data[mesh_ids[i]];

        glVertexArrayVertexBuffer(layout->layout, 0, mesh->vbuffer, 0, sizeof(vertex));
        glVertexArrayElementBuffer(layout->layout, mesh->ibuffer);

        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->num_indices, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    return true;
}

mesh_id mesh_create_primitive_quad_y(mesh_storage *storage, f32 radius, vec4 color)
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

    return mesh_create(storage, data);
}

void mesh_cleanup(mesh_storage *storage, mesh_id id)
{
    _mesh_t *m = &storage->data[id];
    glDeleteBuffers(1, &m->vbuffer);
    glDeleteBuffers(1, &m->ibuffer);

    arrsetlen(storage->data, arrlen(storage->data) - 1);

    arrput(storage->free_ids, id);
}