#include "mesh.h"
#include <glad/glad.h>
#include <stdio.h>

struct model
{
};

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