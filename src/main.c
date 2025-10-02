// ./build.exe && ./game.exe

#define TZL_IMPLEMENTATION
#include "tzl.h"

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include <glad/glad.h>

#define RGFW_IMPLEMENTATION
// #define RGFW_OPENGL
#if defined(DEBUG)
#define RGFW_DEBUG
#endif
#include "../vendor/RGFW.h"

#define STB_DS_IMPLEMENTATION
#include "../vendor/stb_ds.h"

#include "mesh.h"
#include "shader.h"
#include "asset.h"

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

void camera_resize(camera *cam, f32 aspect)
{
    cam->aspect = aspect;

    if (cam->isPerspective)
    {
        mat4x4_perspective(cam->fovy, cam->aspect, cam->nearz, cam->farz, cam->proj);
    }
    else
    {
        float halfWidth = cam->width / 2.0f;
        float halfHeight = (cam->width / aspect) / 2.0f;
        mat4x4_orthographic(-halfWidth, halfWidth, -halfHeight, halfHeight, cam->nearz, cam->farz, cam->proj);
    }
}

void camera_init_perspective(camera *cam, f32 fovy, f32 aspect, f32 nearz, f32 farz)
{
    mat4x4_identity(cam->view);

    cam->isPerspective = true;
    cam->fovy = fovy;
    cam->nearz = nearz;
    cam->farz = farz;

    camera_resize(cam, aspect);
}

void camera_init_ortho(camera *cam, f32 width, f32 aspect, f32 nearz, f32 farz)
{
    mat4x4_identity(cam->view);

    cam->isPerspective = false;
    cam->width = width;
    cam->nearz = nearz;
    cam->farz = farz;

    camera_resize(cam, aspect);
}

void camera_look_at(camera *cam, vec3 eye, vec3 target, vec3 up)
{
    mat4x4_identity(cam->view);
    mat4x4_look_at(eye, target, up, cam->view);
}

typedef size entity_id;
typedef struct transform
{
    entity_id key;

    vec3 pos;
    vec3 scale;
    vec3 rot;
} transform;

typedef struct model
{
    entity_id key;

    shader_id shader;
    mesh_id mesh;
} model;

typedef struct world
{
    entity_id next_entity;
    transform *transform_map;
    model *model_map;
} world;

mesh_id create_test_quad(mesh_storage *storage)
{
    mesh_src_data triangle_src_data = {0};
    arrput(triangle_src_data.vertices, ((vertex){.pos = {-0.5f, -0.5f, 0.0f}, .col = {0.9f, 0.1f, 0.1f, 1.0f}}));
    arrput(triangle_src_data.vertices, ((vertex){.pos = {-0.5f, 0.5f, 0.0f}, .col = {0.1f, 0.9f, 0.1f, 1.0f}}));
    arrput(triangle_src_data.vertices, ((vertex){.pos = {0.5f, 0.5f, 0.0f}, .col = {0.1f, 0.1f, 0.9f, 1.0f}}));
    arrput(triangle_src_data.vertices, ((vertex){.pos = {0.5f, -0.5f, 0.0f}, .col = {0.5f, 0.5f, 0.5f, 1.0f}}));
    arrput(triangle_src_data.indices, 0);
    arrput(triangle_src_data.indices, 1);
    arrput(triangle_src_data.indices, 2);
    arrput(triangle_src_data.indices, 0);
    arrput(triangle_src_data.indices, 2);
    arrput(triangle_src_data.indices, 3);

    return mesh_create(storage, triangle_src_data);
}

entity_id archtype_model(world *world, transform t, model m)
{
    entity_id id = ++world->next_entity;

    t.key = id;
    m.key = id;

    hmputs(world->transform_map, t);
    hmputs(world->model_map, m);

    return id;
}

void sys_render_models(world *w, camera c, vertex_layout vl, mesh_storage *meshes, shader_storage *shaders)
{
    struct query
    {
        model m;
        transform t;
    } *query_result = NULL;

    for (size i = 0; i < hmlen(w->model_map); i++)
    {
        // select * from models m join transform t where m.key is t.key
        model m = w->model_map[i];
        transform *p_t = hmgetp(w->transform_map, m.key);
        if (p_t)
        {
            arrput(query_result, ((struct query){m, *p_t}));
        }
    }

    global_matrix_block matrices = {0};
    memcpy(matrices.view_mat, c.view, sizeof(mat4x4));
    memcpy(matrices.proj_mat, c.proj, sizeof(mat4x4));

    for (size i = 0; i < arrlen(query_result); i++)
    {
        struct query *item = &query_result[i];
        mat4x4 world, t, r, s;
        mat4x4_identity(world);
        vec3_create_translation(item->t.pos, t);
        mat4x4_mul(world, t, world);

        // TODO WT: Rotation from euler (update tzl with in place versions)

        memcpy(matrices.model_mat, world, sizeof(mat4x4));
        shader_use(shaders, item->m.shader, matrices);

        mesh_draw(meshes, vl, &item->m.mesh, 1);
    }
}

typedef struct app_settings
{
    size width;
    size height;
} app_settings;

const char *const APP_SETTINGS_PATH = "./appSettings.bin";

int main(int argc, char **argv)
{
    printf(argv[0]);
    RGFW_windowFlags flags = 0;
    flags |= RGFW_windowCenter;

    app_settings app_settings = {0};
    char *app_settings_raw;
    size app_settings_raw_len;

    if (tzl_load_file(APP_SETTINGS_PATH, &app_settings_raw, &app_settings_raw_len) && sizeof(app_settings) == app_settings_raw_len)
    {
        memcpy_s(&app_settings, sizeof(app_settings), app_settings_raw, app_settings_raw_len);
    }
    else
    {
        app_settings = (struct app_settings){
            .width = 500,
            .height = 500,
        };
    }

    RGFW_windowFlags w_flags = 0 | RGFW_windowCenter;
    RGFW_window *win = RGFW_createWindow("name", RGFW_RECT(100, 100, app_settings.width, app_settings.height), w_flags);
    if (!win)
        return -1;

    if (!gladLoadGL())
        return -1;

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    assets assets;
    assets_create_alloc(&assets);

    vertex_layout standard_layout;
    if (!vertex_layout_create(&standard_layout))
        return -1;

    shader_storage shaders = shader_storage_init();
    mesh_storage meshes = mesh_storage_init();

    shader_id global_shader = shader_load_src(&shaders, "shader.vert", "shader.frag");

    world w = {0};
    archtype_model(
        &w,
        (transform){.pos = {0.0f, 0.5f, 0.0f}, .rot = {0}, .scale = {1, 1, 1}},
        (model){.mesh = create_test_quad(&meshes), .shader = global_shader});

    archtype_model(
        &w,
        (transform){.pos = {0}, .rot = {0}, .scale = {1, 1, 1}},
        (model){.mesh = mesh_create_primitive_quad_y(&meshes, 3.0f, (vec4){.2f, 0.2f, 0.2f, 1.0f}), .shader = global_shader});

    camera cam;
    // camera_init_perspective(&cam, deg_to_radf(60.0f), (float)win->r.w / win->r.h, 0.01f, 1000.0f);
    camera_init_ortho(&cam, 10.0f, (float)win->r.w / win->r.h, 0.01f, 1000.0f);

    timer t = timer_init();

    float angle = 0;

    while (RGFW_window_shouldClose(win) == RGFW_FALSE)
    {
        while (RGFW_window_checkEvent(win))
        {
            if (win->event.type == RGFW_quit || RGFW_isPressed(win, RGFW_escape))
            {
                break;
            }

            if (win->event.type == RGFW_windowResized)
            {
                glViewport(0, 0, win->r.w, win->r.h);
                camera_resize(&cam, (f32)win->r.w / (f32)win->r.h);
            }
        }

        timer_tick(&t);

        const char *fmt = "C Game | FPS: %.0f";
        double fps = 1.0 / t.delta_seconds_smoothed;
        size title_len = snprintf(NULL, 0, fmt, fps) + 1;
        char *title = alloca(title_len * sizeof(char));
        snprintf(title, title_len, fmt, fps);

        RGFW_window_setName(win, title);

        angle += t.delta_seconds;
        float radius = 5.0f;

        camera_look_at(
            &cam,
            (vec3){cosf(angle) * radius, radius / 2.0f, -sinf(angle) * radius},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){0.0f, 1.0f, 0.0f});

        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDisable(GL_CULL_FACE);

        sys_render_models(&w, cam, standard_layout, &meshes, &shaders);

        glEnable(GL_CULL_FACE);

        RGFW_window_swapBuffers(win);
    }

    app_settings.width = win->r.w;
    app_settings.height = win->r.h;

    FILE *f = fopen(APP_SETTINGS_PATH, "w");
    if (f)
    {
        size written = fwrite(&app_settings, sizeof(app_settings), 1, f);
        fclose(f);
    }
    else
    {
        TZL_LOG_ERROR("Could not open/create %s to write appsettings state", APP_SETTINGS_PATH);
        fclose(f);
        return tzl_exit_code_fwrite_error;
    }

    shader_storage_cleanup(&shaders);
    mesh_storage_cleanup(&meshes);

    assets_free(assets);

    RGFW_window_close(win);

    return 0;
}