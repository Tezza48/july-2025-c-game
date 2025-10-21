#pragma once
#include "tzl.h"
#include <glad/glad.h>

typedef struct texture
{
    int width;
    int height;
    GLuint texture;
} texture;

texture texture_load_file(const char *filename);
void texture_delete(texture tex);
