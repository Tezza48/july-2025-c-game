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
#include "asset.h"

typedef struct camera
{
    tzl_mat4x4 proj;
    tzl_mat4x4 rot_mat;

    tzl_vec4 pos;
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

void camera_init(camera *cam)
{
    mat4x4_identity(cam->proj);
    mat4x4_identity(cam->rot_mat);
}

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

    if (!assets_load_shader(assets, "shader", "shader.vert", "shader.frag", NULL))
        return -1;

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

        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);
        shader_use(assets_get_shader(assets, "shader"));

        mesh *meshes = NULL;
        arrput(meshes, triangle_mesh);
        mesh_draw(standard_layout, meshes);

        RGFW_window_swapBuffers(win);
    }

    assets_free(assets);

    RGFW_window_close(win);

    return 0;
}