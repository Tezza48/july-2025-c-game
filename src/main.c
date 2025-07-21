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

int main(int argc, char **argv)
{
    RGFW_window *win = RGFW_createWindow("name", RGFW_RECT(100, 100, 500, 500), (uint64_t)0);
    if (!win)
        return -1;

    if (!gladLoadGL())
        return -1;

    // Load the extension to allow spv

    // Load shader spv src


    while (RGFW_window_shouldClose(win) == RGFW_FALSE)
    {
        while (RGFW_window_checkEvent(win))
        {
            if (win->event.type == RGFW_quit || RGFW_isPressed(win, RGFW_escape))
            {
                break;
            }
        }

        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);



        RGFW_window_swapBuffers(win);
    }

    RGFW_window_close(win);

    return 0;
}