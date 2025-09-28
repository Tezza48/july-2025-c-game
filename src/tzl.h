#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

// Error logging
#ifndef TZL_NO_ERROR
#include <stdio.h>
/// Log an error to stderr
#define TZL_LOG_ERROR(msg, ...) fprintf(stderr, "TZL Error (%s:%d): " msg "\n", __FILE_NAME__, __LINE__, ##__VA_ARGS__)
#endif

// Timer functionality
#pragma region Timer
#include <time.h>
typedef struct tzl_timer
{
    double app_total_seconds;
    double last_seconds;
    double curr_seconds;
    double delta_seconds;
    double delta_seconds_smoothed;
} tzl_timer;

tzl_timer tzl_timer_init();
void tzl_timer_tick(tzl_timer *timer);

#pragma endregion Timer

// Convenient primitives
#pragma region Primitives
typedef size_t tzl_size;

typedef int8_t tzl_i8;
typedef int16_t tzl_i16;
typedef int32_t tzl_i32;
typedef int64_t tzl_i64;

typedef uint8_t tzl_u8;
typedef uint16_t tzl_u16;
typedef uint32_t tzl_u32;
typedef uint64_t tzl_u64;

typedef float tzl_f32;
typedef double tzl_f64;

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif
typedef bool tzl_bool;

#pragma endregion Primitives

/*
    Maths

    OpenGL style maths library.

    All vectors and matrices are column major.
    Vectors are stored as arrays of floats, with the first element being the x component, the second being y, and so on.
    Matrices are stored as arrays of vec4, with the first element being the first column, the second being the second column, and so on.
    The functions are designed to be used with the vec and mat types defined below.

    All angles are in radians.


*/
#include <math.h>

#define TZL_PI 3.141592653589793
#define TZL_EPSILON 1e-6f

static inline tzl_bool tzl_f32_equal(tzl_f32 lhs, tzl_f32 rhs)
{
    return fabsf(lhs - rhs) < TZL_EPSILON;
}
static inline void tzl_f32_lerp(tzl_f32 lhs, tzl_f32 rhs, tzl_f32 t, tzl_f32 *out)
{
    *out = lhs + (rhs - lhs) * t;
}

typedef float tzl_vec2[2];
typedef float tzl_vec3[3];
typedef float tzl_vec4[4];

static inline void tzl_vec2_add(tzl_vec2 lhs, tzl_vec2 rhs, tzl_vec2 out)
{
    out[0] = lhs[0] + rhs[0];
    out[1] = lhs[1] + rhs[1];
}
static inline void tzl_vec2_sub(tzl_vec2 lhs, tzl_vec2 rhs, tzl_vec2 out)
{
    out[0] = lhs[0] - rhs[0];
    out[1] = lhs[1] - rhs[1];
}
static inline void tzl_vec2_scalar_mul(tzl_vec2 lhs, tzl_vec2 rhs, tzl_vec2 out)
{
    out[0] = lhs[0] * rhs[0];
    out[1] = lhs[1] * rhs[1];
}
static inline void tzl_vec2_scalar_div(tzl_vec2 lhs, tzl_vec2 rhs, tzl_vec2 out)
{
    out[0] = lhs[0] / rhs[0];
    out[1] = lhs[1] / rhs[1];
}
static inline void tzl_vec2_length(tzl_vec2 v, tzl_f32 *out)
{
    *out = sqrtf(v[0] * v[0] + v[1] * v[1]);
}
static inline void tzl_vec2_length_squared(tzl_vec2 v, tzl_f32 *out)
{
    *out = v[0] * v[0] + v[1] * v[1];
}
static inline void tzl_vec2_normalize(tzl_vec2 v, tzl_vec2 out)
{
    tzl_f32 length;
    tzl_vec2_length(v, &length);
    if (length > TZL_EPSILON)
    {
        length = 1.0f / length;
        out[0] = v[0] * length;
        out[1] = v[1] * length;
    }
    else
    {
        out[0] = 0.0f;
        out[1] = 0.0f;
    }
}
static inline void tzl_vec2_dot(tzl_vec2 lhs, tzl_vec2 rhs, tzl_f32 *out)
{
    *out = lhs[0] * rhs[0] + lhs[1] * rhs[1];
}
static inline void tzl_vec2_lerp(tzl_vec2 lhs, tzl_vec2 rhs, tzl_f32 t, tzl_vec2 out)
{
    tzl_f32_lerp(lhs[0], rhs[0], t, &out[0]);
    tzl_f32_lerp(lhs[1], rhs[1], t, &out[1]);
}
static inline bool tzl_vec2_equal(tzl_vec2 lhs, tzl_vec2 rhs)
{
    return (fabsf(lhs[0] - rhs[0]) < TZL_EPSILON) && (fabsf(lhs[1] - rhs[1]) < TZL_EPSILON);
}
static inline void tzl_vec2_extend(tzl_vec2 v, tzl_f32 z, tzl_vec3 out)
{
    out[0] = v[0];
    out[1] = v[1];
    out[2] = z;
}

static inline void tzl_vec3_add(tzl_vec3 lhs, tzl_vec3 rhs, tzl_vec3 out)
{
    out[0] = lhs[0] + rhs[0];
    out[1] = lhs[1] + rhs[1];
    out[2] = lhs[2] + rhs[2];
}
static inline void tzl_vec3_sub(tzl_vec3 lhs, tzl_vec3 rhs, tzl_vec3 out)
{
    out[0] = lhs[0] - rhs[0];
    out[1] = lhs[1] - rhs[1];
    out[2] = lhs[2] - rhs[2];
}
static inline void tzl_vec3_scalar_mul(tzl_vec3 lhs, tzl_vec3 rhs, tzl_vec3 out)
{
    out[0] = lhs[0] * rhs[0];
    out[1] = lhs[1] * rhs[1];
    out[2] = lhs[2] * rhs[2];
}
static inline void tzl_vec3_scalar_div(tzl_vec3 lhs, tzl_vec3 rhs, tzl_vec3 out)
{
    out[0] = lhs[0] / rhs[0];
    out[1] = lhs[1] / rhs[1];
    out[2] = lhs[2] / rhs[2];
}
static inline void tzl_vec3_length(tzl_vec3 v, tzl_f32 *out)
{
    *out = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
static inline void tzl_vec3_length_squared(tzl_vec3 v, tzl_f32 *out)
{
    *out = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}
static inline void tzl_vec3_normalize(tzl_vec3 v, tzl_vec3 out)
{
    tzl_f32 length;
    tzl_vec3_length(v, &length);
    if (length > TZL_EPSILON)
    {
        length = 1.0f / length;
        out[0] = v[0] * length;
        out[1] = v[1] * length;
        out[2] = v[2] * length;
    }
    else
    {
        out[0] = 0.0f;
        out[1] = 0.0f;
        out[2] = 0.0f;
    }
}
static inline void tzl_vec3_dot(tzl_vec3 lhs, tzl_vec3 rhs, tzl_f32 *out)
{
    *out = lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
}
static inline void tzl_vec3_cross(tzl_vec3 lhs, tzl_vec3 rhs, tzl_vec3 out)
{
    out[0] = lhs[1] * rhs[2] - lhs[2] * rhs[1];
    out[1] = lhs[2] * rhs[0] - lhs[0] * rhs[2];
    out[2] = lhs[0] * rhs[1] - lhs[1] * rhs[0];
}
static inline void tzl_vec3_lerp(tzl_vec3 lhs, tzl_vec3 rhs, tzl_f32 t, tzl_vec3 out)
{
    tzl_f32_lerp(lhs[0], rhs[0], t, &out[0]);
    tzl_f32_lerp(lhs[1], rhs[1], t, &out[1]);
    tzl_f32_lerp(lhs[2], rhs[2], t, &out[2]);
}
static inline bool tzl_vec3_equal(tzl_vec3 lhs, tzl_vec3 rhs)
{
    return (fabsf(lhs[0] - rhs[0]) < TZL_EPSILON) && (fabsf(lhs[1] - rhs[1]) < TZL_EPSILON) && (fabsf(lhs[2] - rhs[2]) < TZL_EPSILON);
}

static inline void tzl_vec4_add(tzl_vec4 lhs, tzl_vec4 rhs, tzl_vec4 out)
{
    out[0] = lhs[0] + rhs[0];
    out[1] = lhs[1] + rhs[1];
    out[2] = lhs[2] + rhs[2];
    out[3] = lhs[3] + rhs[3];
}
static inline void tzl_vec4_sub(tzl_vec4 lhs, tzl_vec4 rhs, tzl_vec4 out)
{
    out[0] = lhs[0] - rhs[0];
    out[1] = lhs[1] - rhs[1];
    out[2] = lhs[2] - rhs[2];
    out[3] = lhs[3] - rhs[3];
}
static inline void tzl_vec4_scalar_mul(tzl_vec4 lhs, tzl_vec4 rhs, tzl_vec4 out)
{
    out[0] = lhs[0] * rhs[0];
    out[1] = lhs[1] * rhs[1];
    out[2] = lhs[2] * rhs[2];
    out[3] = lhs[3] * rhs[3];
}
static inline void tzl_vec4_scalar_div(tzl_vec4 lhs, tzl_vec4 rhs, tzl_vec4 out)
{
    out[0] = lhs[0] / rhs[0];
    out[1] = lhs[1] / rhs[1];
    out[2] = lhs[2] / rhs[2];
    out[3] = lhs[3] / rhs[3];
}
static inline void tzl_vec4_length(tzl_vec4 v, tzl_f32 *out)
{
    *out = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]);
}
static inline void tzl_vec4_length_squared(tzl_vec4 v, tzl_f32 *out)
{
    *out = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
}
static inline void tzl_vec4_normalize(tzl_vec4 v, tzl_vec4 out)
{
    tzl_f32 length;
    tzl_vec4_length(v, &length);
    if (length > TZL_EPSILON)
    {
        length = 1.0f / length;
        out[0] = v[0] * length;
        out[1] = v[1] * length;
        out[2] = v[2] * length;
        out[3] = v[3] * length;
    }
    else
    {
        out[0] = 0.0f;
        out[1] = 0.0f;
        out[2] = 0.0f;
        out[3] = 0.0f;
    }
}
static inline void tzl_vec4_dot(tzl_vec4 lhs, tzl_vec4 rhs, tzl_f32 *out)
{
    *out = lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
}
static inline void tzl_vec4_lerp(tzl_vec4 lhs, tzl_vec4 rhs, tzl_f32 t, tzl_vec4 out)
{
    tzl_f32_lerp(lhs[0], rhs[0], t, &out[0]);
    tzl_f32_lerp(lhs[1], rhs[1], t, &out[1]);
    tzl_f32_lerp(lhs[2], rhs[2], t, &out[2]);
    tzl_f32_lerp(lhs[3], rhs[3], t, &out[3]);
}
static inline bool tzl_vec4_equal(tzl_vec4 lhs, tzl_vec4 rhs)
{
    return (fabsf(lhs[0] - rhs[0]) < TZL_EPSILON) && (fabsf(lhs[1] - rhs[1]) < TZL_EPSILON) && (fabsf(lhs[2] - rhs[2]) < TZL_EPSILON) && (fabsf(lhs[3] - rhs[3]) < TZL_EPSILON);
}

// Column Major Matrix
typedef tzl_vec4 tzl_mat4x4[4];

static inline void tzl_mat4x4_identity(tzl_mat4x4 m)
{
    m[0][0] = 1.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 1.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 1.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}
static inline void tzl_mat4x4_zero(tzl_mat4x4 m)
{
    m[0][0] = 0.0f;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = 0.0f;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = 0.0f;
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 0.0f;
}
static inline void tzl_mat4x4_mul(tzl_mat4x4 lhs, tzl_mat4x4 rhs, tzl_mat4x4 out)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            out[i][j] = 0.0f;
            for (int k = 0; k < 4; ++k)
            {
                out[i][j] += lhs[i][k] * rhs[k][j];
            }
        }
    }
}
static inline void tzl_mat4x4_mul_vec4(tzl_mat4x4 m, tzl_vec4 v, tzl_vec4 out)
{
    out[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];
    out[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];
    out[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];
    out[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];
}
static inline void tzl_mat4x4_transpose(tzl_mat4x4 m, tzl_mat4x4 out)
{
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            out[i][j] = m[j][i];
        }
    }
}
static inline void tzl_mat4x4_inverse(tzl_mat4x4 m, tzl_mat4x4 out)
{
    // Somewhat ok implementation of matrix inversion
    tzl_mat4x4 temp;
    temp[0][0] = m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[2][3] * m[3][2] - m[2][1] * m[1][2] * m[3][3] + m[2][1] * m[1][3] * m[3][2] + m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[1][3] * m[2][2];
    temp[0][1] = -m[1][0] * m[2][2] * m[3][3] + m[1][0] * m[2][3] * m[3][2] + m[2][0] * m[1][2] * m[3][3] - m[2][0] * m[1][3] * m[3][2] - m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[1][3] * m[2][2];
    temp[0][2] = m[1][0] * m[2][1] * m[3][3] - m[1][0] * m[2][3] * m[3][1] - m[2][0] * m[1][1] * m[3][3] + m[2][0] * m[1][3] * m[3][1] + m[3][0] * m[1][1] * m[2][3] - m[3][0] * m[1][3] * m[2][1];
    temp[0][3] = -m[1][0] * m[2][1] * m[3][2] + m[1][0] * m[2][2] * m[3][1] + m[2][0] * m[1][1] * m[3][2] - m[2][0] * m[1][2] * m[3][1] - m[3][0] * m[1][1] * m[2][2] + m[3][0] * m[1][2] * m[2][1];
    temp[1][0] = -m[0][1] * m[2][2] * m[3][3] + m[0][1] * m[2][3] * m[3][2] + m[2][1] * m[0][2] * m[3][3] - m[2][1] * m[0][3] * m[3][2] - m[3][1] * m[0][2] * m[2][3] + m[3][1] * m[0][3] * m[2][2];
    temp[1][1] = m[0][0] * m[2][2] * m[3][3] - m[0][0] * m[2][3] * m[3][2] - m[2][0] * m[0][2] * m[3][3] + m[2][0] * m[0][3] * m[3][2] + m[3][0] * m[0][2] * m[2][3] - m[3][0] * m[0][3] * m[2][2];
    temp[1][2] = -m[0][0] * m[2][1] * m[3][3] + m[0][0] * m[2][3] * m[3][1] + m[2][0] * m[0][1] * m[3][3] - m[2][0] * m[0][3] * m[3][1] - m[3][0] * m[0][1] * m[2][3] + m[3][0] * m[0][3] * m[2][1];
    temp[1][3] = m[0][0] * m[2][1] * m[3][2] - m[0][0] * m[2][2] * m[3][1] - m[2][0] * m[0][1] * m[3][2] + m[2][0] * m[0][2] * m[3][1] + m[3][0] * m[0][1] * m[2][2] - m[3][0] * m[0][2] * m[2][1];
    temp[2][0] = m[0][1] * m[1][2] * m[3][3] - m[0][1] * m[1][3] * m[3][2] - m[1][1] * m[0][2] * m[3][3] + m[1][1] * m[0][3] * m[3][2] + m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[0][3] * m[1][2];
    temp[2][1] = -m[0][0] * m[1][2] * m[3][3] + m[0][0] * m[1][3] * m[3][2] + m[1][0] * m[0][2] * m[3][3] - m[1][0] * m[0][3] * m[3][2] - m[3][0] * m[0][2] * m[1][3] + m[3][0] * m[0][3] * m[1][2];
    temp[2][2] = m[0][0] * m[1][1] * m[3][3] - m[0][0] * m[1][3] * m[3][1] - m[1][0] * m[0][1] * m[3][3] + m[1][0] * m[0][3] * m[3][1] + m[3][0] * m[0][1] * m[1][3] - m[3][0] * m[0][3] * m[1][1];
    temp[2][3] = -m[0][0] * m[1][1] * m[3][2] + m[0][0] * m[1][2] * m[3][1] + m[1][0] * m[0][1] * m[3][2] - m[1][0] * m[0][2] * m[3][1] - m[3][0] * m[0][1] * m[1][2] + m[3][0] * m[0][2] * m[1][1];
    temp[3][0] = -m[0][1] * m[1][2] * m[2][3] + m[0][1] * m[1][3] * m[2][2] + m[1][1] * m[0][2] * m[2][3] - m[1][1] * m[0][3] * m[2][2] - m[2][1] * m[0][2] * m[1][3] + m[2][1] * m[0][3] * m[1][2];
    temp[3][1] = m[0][0] * m[1][2] * m[2][3] - m[0][0] * m[1][3] * m[2][2] - m[1][0] * m[0][2] * m[2][3] + m[1][0] * m[0][3] * m[2][2] + m[2][0] * m[0][2] * m[1][3] - m[2][0] * m[0][3] * m[1][2];
    temp[3][2] = -m[0][0] * m[1][1] * m[2][3] + m[0][0] * m[1][3] * m[2][1] + m[1][0] * m[0][1] * m[2][3] - m[1][0] * m[0][3] * m[2][1] - m[2][0] * m[0][1] * m[1][3] + m[2][0] * m[0][3] * m[1][1];
    temp[3][3] = m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[1][2] * m[2][1] - m[1][0] * m[0][1] * m[2][2] + m[1][0] * m[0][2] * m[2][1] + m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[0][2] * m[1][1];

    tzl_f32 det = m[0][0] * temp[0][0] + m[0][1] * temp[1][0] + m[0][2] * temp[2][0] + m[0][3] * temp[3][0];
    if (fabsf(det) < TZL_EPSILON)
    {
        // Matrix is singular, cannot be inverted
        tzl_mat4x4_zero(out);
        return;
    }

    det = 1.0f / det;
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            out[i][j] = temp[i][j] * det;
        }
    }
}

static inline void tzl_vec3_create_translation(tzl_vec3 translation, tzl_mat4x4 out)
{
    tzl_mat4x4_identity(out);
    out[0][3] = translation[0];
    out[1][3] = translation[1];
    out[2][3] = translation[2];
}
static inline void tzl_vec3_create_scale(tzl_vec3 scale, tzl_mat4x4 out)
{
    tzl_mat4x4_identity(out);
    out[0][0] = scale[0];
    out[1][1] = scale[1];
    out[2][2] = scale[2];
}
static inline void tzl_vec3_create_rotation_x(tzl_f32 angle, tzl_mat4x4 out)
{
    tzl_mat4x4_identity(out);
    out[1][1] = cosf(angle);
    out[1][2] = -sinf(angle);
    out[2][1] = sinf(angle);
    out[2][2] = cosf(angle);
}
static inline void tzl_vec3_create_rotation_y(tzl_f32 angle, tzl_mat4x4 out)
{
    tzl_mat4x4_identity(out);
    out[0][0] = cosf(angle);
    out[0][2] = sinf(angle);
    out[2][0] = -sinf(angle);
    out[2][2] = cosf(angle);
}
static inline void tzl_vec3_create_rotation_z(tzl_f32 angle, tzl_mat4x4 out)
{
    tzl_mat4x4_identity(out);
    out[0][0] = cosf(angle);
    out[0][1] = -sinf(angle);
    out[1][0] = sinf(angle);
    out[1][1] = cosf(angle);
}
static inline void tzl_vec3_create_rotation(tzl_vec3 axis, tzl_f32 angle, tzl_mat4x4 out)
{
    tzl_f32 c = cosf(angle);
    tzl_f32 s = sinf(angle);
    tzl_f32 t = 1.0f - c;

    out[0][0] = t * axis[0] * axis[0] + c;
    out[0][1] = t * axis[0] * axis[1] - s * axis[2];
    out[0][2] = t * axis[0] * axis[2] + s * axis[1];
    out[0][3] = 0.0f;

    out[1][0] = t * axis[1] * axis[0] + s * axis[2];
    out[1][1] = t * axis[1] * axis[1] + c;
    out[1][2] = t * axis[1] * axis[2] - s * axis[0];
    out[1][3] = 0.0f;

    out[2][0] = t * axis[2] * axis[0] - s * axis[1];
    out[2][1] = t * axis[2] * axis[1] + s * axis[0];
    out[2][2] = t * axis[2] * axis[2] + c;
    out[2][3] = 0.0f;

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
}

static inline void tzl_mat4x4_look_at(tzl_vec3 eye, tzl_vec3 target, tzl_vec3 up, tzl_mat4x4 out)
{
    tzl_vec3 forward, right, upn;
    tzl_vec3_sub(target, eye, forward);
    tzl_vec3_normalize(forward, forward);

    tzl_vec3_cross(forward, up, right);
    tzl_vec3_normalize(right, right);

    tzl_vec3_cross(right, forward, upn);

    out[0][0] = right[0];
    out[1][0] = right[1];
    out[2][0] = right[2];
    out[3][0] = 0.0f;

    out[0][1] = upn[0];
    out[1][1] = upn[1];
    out[2][1] = upn[2];
    out[3][1] = 0.0f;

    out[0][2] = -forward[0];
    out[1][2] = -forward[1];
    out[2][2] = -forward[2];
    out[3][2] = 0.0f;

    tzl_f32 temp_dot;
    tzl_vec3_dot(right, eye, &temp_dot);
    out[0][3] = -temp_dot;

    tzl_vec3_dot(upn, eye, &temp_dot);
    out[1][3] = -temp_dot;

    tzl_vec3_dot(forward, eye, &temp_dot);
    out[2][3] = temp_dot;

    out[3][3] = 1.0f;
}
static inline void tzl_mat4x4_perspective(tzl_f32 fovy, tzl_f32 aspect, tzl_f32 nearz, tzl_f32 farz, tzl_mat4x4 out)
{
    tzl_f32 f = 1.0f / tanf(fovy / 2.0f);
    out[0][0] = f / aspect;
    out[0][1] = 0.0f;
    out[0][2] = 0.0f;
    out[0][3] = 0.0f;

    out[1][0] = 0.0f;
    out[1][1] = f;
    out[1][2] = 0.0f;
    out[1][3] = 0.0f;

    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = (farz + nearz) / (nearz - farz);
    out[2][3] = (2.0f * farz * nearz) / (nearz - farz);

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = -1.0f;
    out[3][3] = 0.0f;
}
static inline void tzl_mat4x4_orthographic(tzl_f32 left, tzl_f32 right, tzl_f32 bottom, tzl_f32 top, tzl_f32 nearz, tzl_f32 farz, tzl_mat4x4 out)
{
    out[0][0] = 2.0f / (right - left);
    out[0][1] = 0.0f;
    out[0][2] = 0.0f;
    out[0][3] = -(right + left) / (right - left);

    out[1][0] = 0.0f;
    out[1][1] = 2.0f / (top - bottom);
    out[1][2] = 0.0f;
    out[1][3] = -(top + bottom) / (top - bottom);

    out[2][0] = 0.0f;
    out[2][1] = 0.0f;
    out[2][2] = -2.0f / (farz - nearz);
    out[2][3] = -(farz + nearz) / (farz - nearz);

    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
}

// Utility functions
static inline tzl_f32 tzl_deg_to_radf(tzl_f32 deg) { return ((deg) * (TZL_PI / 180.0f)); }
static inline tzl_f32 tzl_rad_to_degf(tzl_f32 rad) { return ((rad) * (180.0f / TZL_PI)); }
static inline tzl_f32 tzl_clampf(tzl_f32 value, tzl_f32 min, tzl_f32 max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

// File IO

tzl_bool tzl_load_file(const char *path, char **buff, tzl_size *buffLen);

#if !defined(TZL_NO_SHORT_NAMES)

#ifndef TZL_NO_ERROR
#define LOG_ERROR TZL_LOG_ERROR
#endif

typedef tzl_size size;

typedef tzl_i8 i8;
typedef tzl_i16 i16;
typedef tzl_i32 i32;
typedef tzl_i64 i64;

typedef tzl_u8 u8;
typedef tzl_u16 u16;
typedef tzl_u32 u32;
typedef tzl_u64 u64;

typedef tzl_f32 f32;
typedef tzl_f64 f64;

// bool is already defined in stdbool.h, so we can use it directly already
// typedef tzl_bool bool;

typedef tzl_vec2 vec2;
typedef tzl_vec3 vec3;
typedef tzl_vec4 vec4;

typedef tzl_mat4x4 mat4x4;

#define f32_lerp tzl_f32_lerp
#define vec2_add tzl_vec2_add
#define vec2_sub tzl_vec2_sub
#define vec2_scalar_mul tzl_vec2_scalar_mul
#define vec2_scalar_div tzl_vec2_scalar_div
#define vec2_length tzl_vec2_length
#define vec2_length_squared tzl_vec2_length_squared
#define vec2_normalize tzl_vec2_normalize
#define vec2_dot tzl_vec2_dot
#define vec2_lerp tzl_vec2_lerp
#define vec2_equal tzl_vec2_equal
#define vec3_add tzl_vec3_add
#define vec3_sub tzl_vec3_sub
#define vec3_scalar_mul tzl_vec3_scalar_mul
#define vec3_scalar_div tzl_vec3_scalar_div
#define vec3_length tzl_vec3_length
#define vec3_length_squared tzl_vec3_length_squared
#define vec3_normalize tzl_vec3_normalize
#define vec3_dot tzl_vec3_dot
#define vec3_cross tzl_vec3_cross
#define vec3_lerp tzl_vec3_lerp
#define vec3_equal tzl_vec3_equal
#define vec4_add tzl_vec4_add
#define vec4_sub tzl_vec4_sub
#define vec4_scalar_mul tzl_vec4_scalar_mul
#define vec4_scalar_div tzl_vec4_scalar_div
#define vec4_length tzl_vec4_length
#define vec4_length_squared tzl_vec4_length_squared
#define vec4_normalize tzl_vec4_normalize
#define vec4_dot tzl_vec4_dot
#define vec4_lerp tzl_vec4_lerp
#define vec4_equal tzl_vec4_equal
#define mat4x4_identity tzl_mat4x4_identity
#define mat4x4_zero tzl_mat4x4_zero
#define mat4x4_mul tzl_mat4x4_mul
#define mat4x4_mul_vec4 tzl_mat4x4_mul_vec4
#define mat4x4_transpose tzl_mat4x4_transpose
#define mat4x4_inverse tzl_mat4x4_inverse
#define vec3_create_translation tzl_vec3_create_translation
#define vec3_create_scale tzl_vec3_create_scale
#define vec3_create_rotation_x tzl_vec3_create_rotation_x
#define vec3_create_rotation_y tzl_vec3_create_rotation_y
#define vec3_create_rotation_z tzl_vec3_create_rotation_z
#define vec3_create_rotation tzl_vec3_create_rotation
#define mat4x4_look_at tzl_mat4x4_look_at
#define mat4x4_perspective tzl_mat4x4_perspective
#define mat4x4_orthographic tzl_mat4x4_orthographic
#define deg_to_radf tzl_deg_to_radf
#define rad_to_degf tzl_rad_to_degf
#define clampf tzl_clampf

#define load_file tzl_load_file

typedef tzl_timer timer;

#define timer_init tzl_timer_init
#define timer_tick tzl_timer_tick

#endif

#ifdef TZL_IMPLEMENTATION
#include <stdio.h>
/// @brief Loads the contents of file at "path". Allocates memory in "buff"
/// @param path Path to the file
/// @param buff Buffer to allocate and store the file in
/// @param buffLen Number of bytes in "buff"
/// @return true if the operation was successful
tzl_bool tzl_load_file(const char *path, char **out_buff, tzl_size *out_buff_len)
{
    tzl_bool ok = false;

    char *buff;
    tzl_size len;

    FILE *f = fopen(path, "rb");
    if (!f)
    {
        TZL_LOG_ERROR("Failed to open file: %s", path);
        ok = false;
        goto cleanup;
    }

    tzl_size size = 0;
    len = 0;

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    buff = calloc(size, sizeof(char));
    if (!buff)
    {
        ok = false;
        goto cleanup;
    }
    fread(buff, 1, size, f);

    len = size;

    *out_buff_len = len;
    *out_buff = buff;
    ok = true;

cleanup:
    if (f)
        fclose(f);

    return ok;
}

tzl_timer tzl_timer_init()
{
    return (tzl_timer){0};
}

void tzl_timer_tick(tzl_timer *timer)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    timer->last_seconds = timer->curr_seconds;
    timer->curr_seconds = ts.tv_sec + ts.tv_nsec / 1e9;
    // Default delta seconds to 0
    timer->delta_seconds =
        timer->last_seconds > 0.0f
            ? timer->curr_seconds - timer->last_seconds
            : 0.0f;
    timer->app_total_seconds += timer->delta_seconds;
    timer->delta_seconds_smoothed = (timer->delta_seconds_smoothed + timer->delta_seconds) / 2.0;
}

#endif