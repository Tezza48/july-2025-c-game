#pragma once
#include "tzl.h"

#include "vendor/RGFW.h"

#include "component.h"

#include "mesh.h"
#include "shader.h"
#include "texture.h"

typedef struct camera
{
    tzl_mat4x4 proj;
    tzl_mat4x4 view;

    bool isPerspective;
    union
    {
        float fovy;
        float width;
    };
    float aspect;
    float nearz;
    float farz;
} camera;

typedef struct app
{
    RGFW_window *win;

    mesh_storage meshes;
    shader_storage shaders;
    texture_storage textures;

    entity_id next_entity;
    transform *transform_map;
    model *model_map;

    vertex_layout standard_layout;
    shader_id global_shader;

    timer timer;

    camera cam;
    f32 cam_angle;

    // "<card_rank>_<card_suit>" OR "back_<color>"
    struct card_texture_map
    {
        char *key;
        texture_id value;
    } *card_textures;
} app;

app app_init(RGFW_window *win);
void app_start(app *app);
void app_resize(app *app);
void app_tick(app *app);
void app_cleanup(app *app);
