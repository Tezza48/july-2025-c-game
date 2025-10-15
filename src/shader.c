#include "shader.h"
#include <glad/glad.h>
#include "vendor/stb_ds.h"

struct _shader_t
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

tzl_bool _shader_init_src(shader_p shader, const char *vert_src, const char *frag_src)
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

void _shader_cleanup(shader_p shader)
{
    glDeleteProgram(shader->program);
}

shader_storage shader_storage_init()
{
    return (shader_storage){0};
}

void shader_storage_cleanup(shader_storage *storage)
{
    for (size i = 0; i < arrlen(storage->data); i++)
    {
        _shader_cleanup(&storage->data[i]);
    }
    arrfree(storage->data);
}

shader_id shader_load_src(shader_storage *storage, const char *vert_src, const char *frag_src)
{
    struct _shader_t new_shader;
    if (!_shader_init_src(&new_shader, vert_src, frag_src))
    {
        TZL_LOG_ERROR("Failed to initialize new shader");
        exit(tzl_exit_code_fopen_error);
    }

    shader_id new_id = arrlen(storage->data);
    arrput(storage->data, new_shader);

    return new_id;
}

void shader_update_resources(global_matrix_block matrices)
{
    glUniformMatrix4fv(0, 1, GL_FALSE, (GLfloat *)matrices.model_mat);
    glUniformMatrix4fv(1, 1, GL_FALSE, (GLfloat *)matrices.view_mat);
    glUniformMatrix4fv(2, 1, GL_FALSE, (GLfloat *)matrices.proj_mat);
}

void shader_use(shader_storage *storage, shader_id shader, global_matrix_block matrices)
{
    shader_p s = &storage->data[shader];
    glUseProgram(s->program);

    shader_update_resources(matrices);
}