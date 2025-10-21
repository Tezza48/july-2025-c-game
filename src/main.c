// ./build.exe && ./game.exe

#define TZL_IMPLEMENTATION
#include "tzl.h"

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include <glad/glad.h>

#include "app.h"

#define STB_DS_IMPLEMENTATION
#include "vendor/stb_ds.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

typedef struct app_settings
{
    size width;
    size height;
} app_settings;

const char *const APP_SETTINGS_PATH = "./appSettings.bin";

int main(int argc, char **argv)
{
    app_settings app_settings = (struct app_settings){
        .width = 1280,
        .height = 720,
    };

    stbi_set_flip_vertically_on_load(true);

    RGFW_windowFlags flags = 0;
    flags |= RGFW_windowCenter;
    flags |= RGFW_windowNoResize;

    RGFW_window *win = RGFW_createWindow("name", RGFW_RECT(100, 100, app_settings.width, app_settings.height), flags);
    if (!win)
        return -1;

    if (!gladLoadGL())
        return -1;

    app app = app_init(win);
    app_start(&app);

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
                app_resize(&app);
        }

        app_tick(&app);

        RGFW_window_swapBuffers(win);
    }

    app_cleanup(&app);

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

    RGFW_window_close(win);

    return 0;
}