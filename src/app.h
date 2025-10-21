#pragma once
#include "tzl.h"

#include "vendor/RGFW.h"

#include "component.h"

#include "mesh.h"
#include "shader.h"
#include "texture.h"

typedef struct card
{
    char *name;
    vec2 pos;
} card;

typedef struct app
{
    RGFW_window *win;

    GLuint card_mesh;

    GLuint vertex_layout;
    GLuint global_shader;

    mesh test_mesh;

    // "<card_rank>_<card_suit>" OR "back_<color>"
    struct card_texture_map
    {
        char *key;

        texture value;
    } *card_textures;

    card *cards;

    timer timer;

    camera cam;
} app;

app app_init(RGFW_window *win);
void app_start(app *app);
void app_resize(app *app);
void app_tick(app *app);
void app_cleanup(app *app);
