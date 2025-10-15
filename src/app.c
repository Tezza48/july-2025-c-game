#include "app.h"

#include <stdlib.h>
#include <glad/glad.h>

#include "vendor/stb_ds.h"

entity_id archtype_model(app *app, transform t, model m)
{
    entity_id id = ++app->next_entity;

    t.key = id;
    m.key = id;

    hmputs(app->transform_map, t);
    hmputs(app->model_map, m);

    return id;
}

#pragma region Systems

void sys_render_models(app *app)
{
    struct query
    {
        model m;
        transform t;
    } *query_result = NULL;

    for (size i = 0; i < hmlen(app->model_map); i++)
    {
        // select * from models m join transform t where m.key is t.key
        model m = app->model_map[i];
        transform *p_t = hmgetp(app->transform_map, m.key);
        if (p_t)
        {
            arrput(query_result, ((struct query){m, *p_t}));
        }
    }

    global_matrix_block matrices = {0};
    memcpy(matrices.view_mat, app->cam.view, sizeof(mat4x4));
    memcpy(matrices.proj_mat, app->cam.proj, sizeof(mat4x4));

    for (size i = 0; i < arrlen(query_result); i++)
    {
        struct query *item = &query_result[i];
        mat4x4 world, t, r, s;
        mat4x4_identity(world);
        vec3_create_translation(item->t.pos, t);
        mat4x4_mul(world, t, world);

        // TODO WT: Rotation from euler (update tzl with in place versions)

        memcpy(matrices.model_mat, world, sizeof(mat4x4));
        shader_use(&app->shaders, item->m.shader, matrices);

        glActiveTexture(GL_TEXTURE0);
        texture_id tex = hmget(app->card_textures, "A_Spades");
        glBindTexture(GL_TEXTURE_2D, texture_get_gl_texture(&app->textures, tex));

        mesh_draw(&app->meshes, app->standard_layout, &item->m.mesh, 1);
    }
}

#pragma endregion Systems

mesh_id create_test_quad(mesh_storage *storage)
{
    mesh_src_data triangle_src_data = {0};
    arrput(triangle_src_data.vertices, ((vertex){.pos = {-0.5f, -0.5f, 0.0f}, .col = {0.9f, 0.1f, 0.1f, 1.0f}, .uv = {0.0f, 0.0f}}));
    arrput(triangle_src_data.vertices, ((vertex){.pos = {-0.5f, 0.5f, 0.0f}, .col = {0.1f, 0.9f, 0.1f, 1.0f}, .uv = {0.0f, 1.0f}}));
    arrput(triangle_src_data.vertices, ((vertex){.pos = {0.5f, 0.5f, 0.0f}, .col = {0.1f, 0.1f, 0.9f, 1.0f}, .uv = {1.0f, 1.0f}}));
    arrput(triangle_src_data.vertices, ((vertex){.pos = {0.5f, -0.5f, 0.0f}, .col = {0.5f, 0.5f, 0.5f, 1.0f}, .uv = {1.0f, 0.0f}}));
    arrput(triangle_src_data.indices, 0);
    arrput(triangle_src_data.indices, 1);
    arrput(triangle_src_data.indices, 2);
    arrput(triangle_src_data.indices, 0);
    arrput(triangle_src_data.indices, 2);
    arrput(triangle_src_data.indices, 3);

    return mesh_create(storage, triangle_src_data);
}

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

app app_init(RGFW_window *win)
{
    app app = {0};
    app.win = win;

    app.shaders = shader_storage_init();
    app.meshes = mesh_storage_init();
    app.textures = (texture_storage){0}; // lazy but the idea is that its gonna zero out anyway

    app.global_shader = shader_load_src(&app.shaders, "shaders/shader.vert", "shaders/shader.frag");

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

            texture_id t = texture_load_file(&app.textures, asset_path);

            hmput(app.card_textures, asset_path, t);
        }
    }

    return app;
}

void app_start(app *app)
{
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (!vertex_layout_create(&app->standard_layout))
    {
        TZL_LOG_ERROR("Failed to create Vertex Layout");
        exit(tzl_exit_code_unknown_error);
    }

    archtype_model(
        app,
        (transform){.pos = {0.0f, 0.5f, 0.0f}, .rot = {0}, .scale = {1, 1, 1}},
        (model){.mesh = create_test_quad(&app->meshes), .shader = app->global_shader});

    archtype_model(
        app,
        (transform){.pos = {0}, .rot = {0}, .scale = {1, 1, 1}},
        (model){.mesh = mesh_create_primitive_quad_y(&app->meshes, 3.0f, (vec4){.2f, 0.2f, 0.2f, 1.0f}), .shader = app->global_shader});

    // camera_init_perspective(&cam, deg_to_radf(60.0f), (float)win->r.w / win->r.h, 0.01f, 1000.0f);
    camera_init_ortho(&app->cam, 10.0f, (float)app->win->r.w / app->win->r.h, 0.01f, 1000.0f);

    app->timer = timer_init();
}

void app_resize(app *app)
{
    glViewport(0, 0, app->win->r.w, app->win->r.h);
    camera_resize(&app->cam, (f32)app->win->r.w / (f32)app->win->r.h);
}

void app_tick(app *app)
{
    timer_tick(&app->timer);

    const char *fmt = "C Game | FPS: %.0f";
    double fps = 1.0 / app->timer.delta_seconds_smoothed;
    size title_len = snprintf(NULL, 0, fmt, fps) + 1;
    char *title = alloca(title_len * sizeof(char));
    snprintf(title, title_len, fmt, fps);

    RGFW_window_setName(app->win, title);

    app->cam_angle += app->timer.delta_seconds;
    float radius = 5.0f;

    camera_look_at(
        &app->cam,
        (vec3){cosf(app->cam_angle) * radius, radius / 2.0f, -sinf(app->cam_angle) * radius},
        (vec3){0.0f, 0.0f, 0.0f},
        (vec3){0.0f, 1.0f, 0.0f});

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);

    sys_render_models(app);

    glEnable(GL_CULL_FACE);
}

void app_cleanup(app *app)
{
    vertex_layout_free(app->standard_layout);

    texture_storage_cleanup(&app->textures);
    shader_storage_cleanup(&app->shaders);
    mesh_storage_cleanup(&app->meshes);
}