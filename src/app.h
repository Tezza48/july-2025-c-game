#pragma once
#include "tzl.h"

#include "vendor/RGFW.h"

#include "component.h"

#include "mesh.h"
#include "shader.h"
#include "texture.h"

typedef struct card
{
    char *texture_name;
    vec2 pos;
    vec2 anchor;
    bool hidden;
} card;

typedef struct app
{
    RGFW_window *win;

    GLuint vertex_layout;
    GLuint global_shader;

    mesh card_mesh;

    // "<card_rank>_<card_suit>" OR "back_<color>"
    struct card_texture_map
    {
        char *key;

        texture value;
    } *sh_card_textures;

    card *cards;

    size deck_card_id;
    size room_card_ids[4];
    size discard_top_card_id;

    timer timer;

    camera cam;
} app;

void app_init(app* app);
void app_start(app *app);
void app_cleanup(app *app);
