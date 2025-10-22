#include "app.h"

#include <stdlib.h>
#include <glad/glad.h>

#include "vendor/stb_ds.h"

void _load_card_textures(app *app)
{

    char *ranks[13] = {
        "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

    char *suits[4] = {
        "Clubs", "Hearts", "Spades", "Diamonds"};

    char *format = "kenney_boardgame/Cards/card%s%s.png";
    size numRanks = sizeof(ranks) / sizeof(ranks[0]);
    size numSuits = sizeof(suits) / sizeof(suits[0]);
    for (size s = 0; s < numSuits; s++)
    {
        char *suit = suits[s];
        for (size r = 0; r < numRanks; r++)
        {
            char *rank = ranks[r];
            char asset_path[1024];
            snprintf(asset_path, sizeof(asset_path), format, suit, rank);

            texture t = texture_load_file(asset_path);

            hmput(app->card_textures, asset_path, t);
        }
    }
}

#pragma region Systems

#pragma endregion Systems

void _camera_resize(camera *cam, f32 aspect)
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

void _camera_init_perspective(camera *cam, f32 fovy, f32 aspect, f32 nearz, f32 farz)
{
    mat4x4_identity(cam->view);

    cam->isPerspective = true;
    cam->fovy = fovy;
    cam->nearz = nearz;
    cam->farz = farz;

    _camera_resize(cam, aspect);
}

void _camera_init_ortho(camera *cam, f32 width, f32 aspect, f32 nearz, f32 farz)
{
    mat4x4_identity(cam->view);

    cam->isPerspective = false;
    cam->width = width;
    cam->nearz = nearz;
    cam->farz = farz;

    _camera_resize(cam, aspect);
}

void _camera_look_at(camera *cam, vec3 eye, vec3 target, vec3 up)
{
    mat4x4_identity(cam->view);
    mat4x4_look_at(eye, target, up, cam->view);
}

void _resize(app *app)
{
    glViewport(0, 0, app->win->r.w, app->win->r.h);
    _camera_resize(&app->cam, (f32)app->win->r.w / (f32)app->win->r.h);
}

void _tick(app *app)
{
    timer_tick(&app->timer);

    const char *fmt = "C Game | FPS: %.0f";
    double fps = 1.0 / app->timer.delta_seconds_smoothed;
    size title_len = snprintf(NULL, 0, fmt, fps) + 1;
    char *title = alloca(title_len * sizeof(char));
    snprintf(title, title_len, fmt, fps);

    RGFW_window_setName(app->win, title);
    float radius = 5.0f;

    _camera_look_at(
        &app->cam,
        (vec3){0.0f, 0.0f, 0.0f},
        (vec3){0.0f, 0.0f, -1.0f},
        (vec3){0.0f, 1.0f, 0.0f});

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);

    glBindVertexArray(app->vertex_layout);

    glUseProgram(app->global_shader);
    global_matrix_block matrices;
    mat4x4_identity(matrices.model_mat);
    memcpy(matrices.view_mat, app->cam.view, sizeof(mat4x4));
    memcpy(matrices.proj_mat, app->cam.proj, sizeof(mat4x4));
    shader_standard_update_resources(matrices);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->card_textures[0].value.texture);

    mesh_bind(app->vertex_layout, &app->test_mesh);
    glDrawElements(GL_TRIANGLES, (GLsizei)app->test_mesh.num_indices, GL_UNSIGNED_INT, 0);

    glEnable(GL_CULL_FACE);
}

app app_create()
{
    app app = {0};

    RGFW_windowFlags flags = 0;
    flags |= RGFW_windowCenter;
    flags |= RGFW_windowNoResize;

    app.win = RGFW_createWindow("name", RGFW_RECT(100, 100, 1280, 720), flags);
    if (!app.win)
    {
        TZL_LOG_ERROR("Failed to create window");
        exit(tzl_exit_code_unknown_error);
    }

    if (!gladLoadGL())
    {
        TZL_LOG_ERROR("Failed to load opengl");
        exit(tzl_exit_code_unknown_error);
    }

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    app.global_shader = shader_load_src("shaders/shader.vert", "shaders/shader.frag");
    app.vertex_layout = vertex_layout_create();

    _load_card_textures(&app);

    app.test_mesh = mesh_create_sprite((f32)app.card_textures[0].value.width, (f32)app.card_textures[0].value.height, (vec4){1.0f, 0.0f, 0.0f, 1.0f});

    _camera_init_ortho(&app.cam, 1280.0f, (float)app.win->r.w / app.win->r.h, -1000.0f, 1000.0f);

    app.timer = timer_init();

    return app;
}

void app_start(app *app)
{
    while (RGFW_window_shouldClose(app->win) == RGFW_FALSE)
    {
        while (RGFW_window_checkEvent(app->win))
        {
            if (app->win->event.type == RGFW_quit || RGFW_isPressed(app->win, RGFW_escape))
            {
                break;
            }

            if (app->win->event.type == RGFW_windowResized)
                _resize(app);
        }

        _tick(app);

        RGFW_window_swapBuffers(app->win);
    }
}

void app_cleanup(app *app)
{
    vertex_layout_free(app->vertex_layout);

    for (size i = 0; i < hmlen(app->card_textures); i++)
        texture_delete(app->card_textures[i].value);
    hmfree(app->card_textures);

    shader_delete(app->global_shader);

    RGFW_window_close(app->win);
}