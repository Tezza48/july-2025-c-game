#pragma once

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