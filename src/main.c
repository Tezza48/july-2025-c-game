#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "../vendor/RGFW.h"

int main(int argc, char **argv)
{
    RGFW_window* win = RGFW_createWindow("name", RGFW_RECT(100, 100, 500, 500), (uint64_t)0);

    while(RGFW_window_shouldClose(win) == RGFW_FALSE) {
        while(RGFW_window_checkEvent(win)) {
            if (win->event.type == RGFW_quit || RGFW_isPressed(win, RGFW_escape)) {
                break;
            }
        }

        RGFW_window_swapBuffers(win);

    }

    RGFW_window_close(win);

    return 0;
}