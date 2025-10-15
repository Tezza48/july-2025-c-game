#include "texture.h"
#include <glad/glad.h>
#include "vendor/stb_image.h"
#include "vendor/stb_ds.h"

struct _texture_t
{
    int width;
    int height;
    GLuint texture;
};

texture_storage texture_storage_init()
{
    return (texture_storage){0};
}
void texture_storage_cleanup(texture_storage *storage)
{
    for (size i = 0; i < arrlen(storage->data); i++)
    {
        glDeleteTextures(1, &storage->data[i].texture);
    }

    arrfree(storage->data);
}

texture_id texture_load_file(texture_storage *storage, const char *filename)
{
    u32 x = 0, y = 0, bpp = 0;
    stbi_uc *data = stbi_load(filename, &x, &y, &bpp, STBI_rgb_alpha);
    if (!data)
    {
        TZL_LOG_ERROR("Failed to load texture: %s", filename);
        exit(tzl_exit_code_fopen_error);
    }

    GLuint texture = 0;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    glObjectLabel(GL_TEXTURE, texture, -1, filename);

    texture_id id = arrlen(storage->data);
    arrput(storage->data, ((struct _texture_t){.width = x, .height = y, .texture = texture}));

    stbi_image_free(data);

    return id;
}

GLuint texture_get_gl_texture(texture_storage *storage, texture_id id)
{
    return storage->data[id].texture;
}