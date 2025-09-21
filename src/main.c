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

void camera_init(camera *cam)
{
    mat4x4_identity(cam->proj);
    mat4x4_identity(cam->rot_mat);
}

int main(int argc, char **argv)
{
    printf(argv[0]);
    RGFW_window *win = RGFW_createWindow("name", RGFW_RECT(100, 100, 500, 500), (uint64_t)0);
    if (!win)
        return -1;

    if (!gladLoadGL())
        return -1;

    assets assets;
    assets_create_alloc(&assets);

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
        }

        glClearColor(0.9f, 0.9f, 0.9f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);
        shader_use(assets_get_shader(assets, "shader"));
        glDrawArrays(GL_TRIANGLES, 0, 3);

        RGFW_window_swapBuffers(win);
    }

    assets_free(assets);

    RGFW_window_close(win);

    return 0;
}