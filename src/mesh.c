#include "mesh.h"
#include <glad/glad.h>
#include <stdio.h>

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

bool mesh_draw(vertex_layout layout, mesh *meshes)
{
    glBindVertexArray(layout->layout);

    for (size i = 0; i < arrlen(meshes); i++)
    {
        mesh mesh = meshes[i];
        glVertexArrayVertexBuffer(layout->layout, 0, mesh->vbuffer, 0, sizeof(vertex));
        glVertexArrayElementBuffer(layout->layout, mesh->ibuffer);

        glDrawElements(GL_TRIANGLES, (GLsizei)mesh->num_indices, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    return true;
}

void mesh_free(mesh mesh)
{
    glDeleteBuffers(1, &mesh->vbuffer);
    glDeleteBuffers(1, &mesh->ibuffer);

    free(mesh);
}

struct shader
{
    GLuint program;
};

bool _load_shader(const char *path, GLenum shader_type, GLuint *out_shader)
{
    bool ok = false;
    char *src_code = NULL;
    GLuint shader = 0;

    shader = glCreateShader(shader_type);
    if (!shader)
    {
        TZL_LOG_ERROR("Failed to create shader: %s", path);
        goto cleanup;
    }

    size_t src_len = 0;
    if (!load_file(path, &src_code, &src_len))
    {
        TZL_LOG_ERROR("Failed to load file: %s", path);
        goto cleanup;
    }

    GLint src_len_gl = (GLint)src_len;
    glShaderSource(shader, 1, (const GLchar *const *)&src_code, &src_len_gl);
    glCompileShader(shader);

    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        GLint logLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

        if (logLen > 1)
        {
            char *log = calloc(logLen, 1);
            if (log)
            {
                glGetShaderInfoLog(shader, logLen, NULL, log);
                TZL_LOG_ERROR("Shader compilation failed: %s", log);
                free(log);
            }
        }
        else
        {
            TZL_LOG_ERROR("Shader compilation failed (no log available): %s", path);
        }
        goto cleanup;
    }

    *out_shader = shader;
    ok = true;

cleanup:
    if (!ok && shader)
        glDeleteShader(shader);
    free(src_code);

    return ok;
}

void shader_use(shader mat)
{
    glUseProgram(mat->program);
}

tzl_bool _shader_init_src(shader shader, const char *vert_src, const char *frag_src)
{
    GLuint program = 0;
    GLuint vertShader = 0;
    GLuint fragShader = 0;
    bool ok = false;

    if (!shader)
    {
        TZL_LOG_ERROR("Shader instance is not valid");
        goto cleanup;
    }

    // Create program
    program = glCreateProgram();
    if (!program)
    {
        TZL_LOG_ERROR("Failed to create program");
        goto cleanup;
    }

    // Load shaders
    if (!_load_shader(vert_src, GL_VERTEX_SHADER, &vertShader) ||
        !_load_shader(frag_src, GL_FRAGMENT_SHADER, &fragShader))
    {
        goto cleanup;
    }

    // Attach and link
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    GLint linkStatus = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1)
        {
            char *log = calloc(logLength, 1);
            if (log)
            {
                glGetProgramInfoLog(program, logLength, NULL, log);
                TZL_LOG_ERROR("Shader program linking failed: %s", log);
                free(log);
            }
        }
        goto cleanup;
    }

    // Clean up shader objects: they are no longer needed after linking
    glDetachShader(program, vertShader);
    glDetachShader(program, fragShader);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    vertShader = fragShader = 0;

    shader->program = program;
    ok = true;

cleanup:
    if (vertShader)
        glDeleteShader(vertShader);
    if (fragShader)
        glDeleteShader(fragShader);

    if (!ok)
    {
        if (program)
            glDeleteProgram(program);
    }

    return ok;
}

tzl_bool shader_load_src(const char *vert_src, const char *frag_src, shader *out_shader)
{
    bool ok = false;

    // Allocate shader
    shader shader = calloc(1, sizeof(*shader));
    if (!shader)
    {
        TZL_LOG_ERROR("Could not allocate memory for material");
        goto cleanup;
    }

    if (!_shader_init_src(shader, vert_src, frag_src))
    {
        goto cleanup;
    }

    *out_shader = shader;
    ok = true;

cleanup:
    if (!ok)
    {
        if (shader)
        {
            free(shader);
        }
    }

    return ok;
}

void _shader_cleanup(shader shader)
{
    glDeleteProgram(shader->program);
}

void shader_free(shader shader)
{
    _shader_cleanup(shader);
    free(shader);
}

// Asset system integration
static inline void _shader_free_asset(void *s)
{
    shader_free((shader)s);
}

bool assets_load_shader(assets assets, const char *name, const char *vert_src, const char *frag_src, shader *out_shader)
{
    bool ok = false;

    shader s;
    if (!shader_load_src(vert_src, frag_src, &s))
    {
        TZL_LOG_ERROR("Failed to load shader src");
        goto cleanup;
    }

    assets_add(assets, name, s, _shader_free_asset);

    if (out_shader)
        *out_shader = s;

    ok = true;

cleanup:
    return ok;
}