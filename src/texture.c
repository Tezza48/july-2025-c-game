#include "texture.h"
#include <glad/glad.h>
#include "vendor/stb_image.h"
#include "vendor/stb_ds.h"

texture texture_load_file(const char *filename)
{
    u32 x = 0, y = 0, bpp = 0;
    stbi_uc *data = stbi_load(filename, &x, &y, &bpp, STBI_rgb_alpha);
    if (!data)
    {
        TZL_LOG_ERROR("Failed to load texture: %s", filename);
        exit(tzl_exit_code_fopen_error);
    }

    GLuint tex = 0;
    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    glObjectLabel(GL_TEXTURE, tex, -1, filename);
    stbi_image_free(data);

    return (texture){
        .width = x,
        .height = y,
        .texture = tex};
}

void texture_delete(texture tex)
{
    glDeleteTextures(1, &tex.texture);
}