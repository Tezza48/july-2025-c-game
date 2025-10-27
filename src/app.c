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

            char key[1024] = {0};

            snprintf(key, sizeof(key), "%s_%s", rank, suit);

            texture t = texture_load_file(asset_path);

            shput(app->sh_card_textures, key, t);
        }
    }

    char *colors[3] = {
        "blue",
        "green",
        "red",
    };
    format = "kenney_boardgame/Cards/cardBack_%s2.png";
    for (size c = 0; c < sizeof(colors) / sizeof(colors[0]); c++)
    {
        char *color = colors[c];
        char asset_path[1024];
        snprintf(asset_path, sizeof(asset_path), format, color);

        char key[1024];
        snprintf(key, sizeof(key), "back_%s", color);

        texture t = texture_load_file(asset_path);

        shput(app->sh_card_textures, key, t);
    }
}

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

void render_card(app *app, card c)
{
    texture tex = shget(app->sh_card_textures, c.texture_name);

    glUseProgram(app->global_shader);
    global_matrix_block matrices;
    mat4x4_identity(matrices.model_mat);
    vec3 pos;
    tzl_vec2_extend(c.pos, 0.0f, pos);
    vec3 anchor_offs = {c.anchor[0] * tex.width, -c.anchor[1] * tex.height, 0.0f};
    pos[0] -= anchor_offs[0];
    pos[1] -= anchor_offs[1];
    vec3_create_translation(pos, matrices.model_mat);
    memcpy(matrices.view_mat, app->cam.view, sizeof(mat4x4));
    memcpy(matrices.proj_mat, app->cam.proj, sizeof(mat4x4));
    shader_standard_update_resources(matrices);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.texture);

    glBindVertexArray(app->vertex_layout);

    mesh_bind(app->vertex_layout, &app->card_mesh);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawElements(GL_TRIANGLES, (GLsizei)app->card_mesh.num_indices, GL_UNSIGNED_INT, 0);
}

void _tick(app *app)
{
    timer_tick(&app->timer);

    // Logical Tick

    // Render Tick
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

    for (size i = 0; i < arrlen(app->cards); i++)
    {
        render_card(app, app->cards[i]);
    }

    glBindVertexArray(app->vertex_layout);
    glEnable(GL_CULL_FACE);
}

void _create_scene(app *app)
{
    app->deck_card_id = arrlen(app->cards);
    arrput(app->cards, ((card){.texture_name = "back_green", .pos = {0.0f, 360.0f}, .anchor = {0.5f, 0}}));


}

void app_init(app *app)
{
    RGFW_windowFlags flags = 0;
    flags |= RGFW_windowCenter;
    flags |= RGFW_windowNoResize;

    app->win = RGFW_createWindow("name", RGFW_RECT(100, 100, 1280, 720), flags);
    if (!app->win)
    {
        TZL_LOG_ERROR("Failed to create window");
        exit(tzl_exit_code_unknown_error);
    }

    if (!gladLoadGL())
    {
        TZL_LOG_ERROR("Failed to load opengl");
        exit(tzl_exit_code_unknown_error);
    }

    // Specifically want the card_textures keys to be stored in arena as entries are never deleted
    sh_new_arena(app->sh_card_textures);

    app->timer = timer_init();

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    app->global_shader = shader_load_src("shaders/shader.vert", "shaders/shader.frag");
    app->vertex_layout = vertex_layout_create();

    _load_card_textures(app);

    app->card_mesh = mesh_create_sprite((f32)app->sh_card_textures[0].value.width, (f32)app->sh_card_textures[0].value.height, (vec4){1.0f, 1.0f, 1.0f, 1.0f});

    _camera_init_ortho(&app->cam, 1280.0f, (float)app->win->r.w / app->win->r.h, -1000.0f, 1000.0f);

    _create_scene(app);
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
    mesh_delete(app->card_mesh);
    shader_delete(app->global_shader);
    vertex_layout_free(app->vertex_layout);

    for (size i = 0; i < shlen(app->sh_card_textures); i++)
    {
        texture_delete(app->sh_card_textures[i].value);
    }
    shfree(app->sh_card_textures);

    arrfree(app->cards);

    RGFW_window_close(app->win);
}