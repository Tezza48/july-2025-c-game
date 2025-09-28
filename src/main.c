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

    float fovy;
    float aspect;
    float nearz;
    float farz;
} camera;

// typedef enum event_type
// {
//     event_resize,
// } event_type;

// typedef void (*event_callback)(event_type event_type, void *context, void *data);
// typedef struct event
// {
//     event_callback callback;
//     void *context
// } event;
// void event_notify(event *events, event_type event_type, void *data)
// {
//     for (size i = 0; i < arrlen(events); i++)
//     {
//         events[i].callback(event_type, events[i].context, data);
//     }
// }

void camera_init(camera *cam, f32 fovy, f32 aspect, f32 nearz, f32 farz)
{
    mat4x4_perspective(fovy, aspect, nearz, farz, cam->proj);
    mat4x4_identity(cam->view);

    cam->fovy = fovy;
    cam->aspect = aspect;
    cam->nearz = nearz;
    cam->farz = farz;
}

void camera_look_at(camera *cam, vec3 eye, vec3 target, vec3 up)
{
    mat4x4_identity(cam->view);
    mat4x4_look_at(eye, target, up, cam->view);
}

// void camera_set_view(camera *cam, vec3 pos, vec4 orientation_quat)
// {
// }

int main(int argc, char **argv)
{
    printf(argv[0]);
    RGFW_windowFlags flags = 0;
    flags |= RGFW_windowCenter;

    // event_callback *events;

    RGFW_window *win = RGFW_createWindow("name", RGFW_RECT(100, 100, 500, 500), (uint64_t)0);
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

    mesh triangle_mesh;
    if (!mesh_create(triangle_src_data, &triangle_mesh))
        return -1;

    mesh floor_mesh;
    if (!mesh_primitive_quad_y(3.0f, (vec4){.2f, 0.2f, 0.2f, 1.0f}, &floor_mesh))
        return -1;

    if (!assets_load_shader(assets, "shader", "shader.vert", "shader.frag", NULL))
        return -1;

    camera cam;
    camera_init(&cam, deg_to_radf(60.0f), (float)win->r.h / win->r.w, 0.01f, 1000.0f);

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
            (vec3){cosf(angle) * radius, radius, -sinf(angle) * radius},
            (vec3){0.0f, 0.0f, 0.0f},
            (vec3){0.0f, 1.0f, 0.0f});

        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4x4 mesh_model;
        vec3_create_translation((vec3){0.0f, 0.5f, 0.0f}, mesh_model);

        global_matrix_block matrices = {0};
        memcpy(matrices.model_mat, mesh_model, sizeof(mat4x4));
        memcpy(matrices.view_mat, cam.view, sizeof(mat4x4));
        memcpy(matrices.proj_mat, cam.proj, sizeof(mat4x4));

        // Not ideal that id be grabbing shader from the cache per pass, id grab it once
        shader_use(assets_get_shader(assets, "shader"), matrices);

        mesh_draw(standard_layout, &triangle_mesh, 1);

        mat4x4_identity(matrices.model_mat);
        shader_use(assets_get_shader(assets, "shader"), matrices);
        mesh_draw(standard_layout, &floor_mesh, 1);

        RGFW_window_swapBuffers(win);
    }

    assets_free(assets);

    RGFW_window_close(win);

    return 0;
}