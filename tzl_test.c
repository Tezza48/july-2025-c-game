// gcc .\tzl_test.c -o tzl_test.exe && .\tzl_test.exe

#include <stdio.h>
#define TZL_NO_SHORT_NAMES
#define TZL_IMPLEMENTATION
#include "src/tzl.h"

#define run_test(test, passed)                         \
    if ((test)())                                      \
    {                                                  \
        printf("\t\x1b[32mPASS\x1b[0m %s\n", (#test)); \
        (passed) |= true;                              \
    }                                                  \
    else                                               \
    {                                                  \
        printf("\t\x1b[31mFAIL\x1b[0m %s\n", (#test)); \
        (passed) &= false;                             \
    };

bool it_tests_equality_of_f32(void)
{
    tzl_f32 a = 0.1f;
    tzl_f32 b = 0.1f;
    return tzl_f32_equal(a, b);
}

bool it_lerps_f32(void)
{
    tzl_f32 a = 0.0f;
    tzl_f32 b = 1.0f;
    tzl_f32 t = 0.5f;
    tzl_f32 result;

    tzl_f32_lerp(a, b, t, &result);

    return tzl_f32_equal(result, 0.5f) && !tzl_f32_equal(result, 0.6f);
}

bool it_tests_equality_of_vec2(void)
{
    tzl_vec2 a = {1.0f, 2.0f};
    tzl_vec2 b = {1.0f, 2.0f};
    return tzl_vec2_equal(a, b);
}

bool it_adds_vec2_to_vec2(void)
{
    tzl_vec2 a = {1.0f, 2.0f};
    tzl_vec2 b = {3.0f, 4.0f};

    tzl_vec2 result;
    tzl_vec2_add(a, b, result);

    tzl_vec2 expected = {4.0f, 6.0f};
    return tzl_vec2_equal(result, expected);
}

bool it_subs_vec2_from_vec2(void)
{
    tzl_vec2 a = {1.0f, 2.0f};
    tzl_vec2 b = {3.0f, 4.0f};

    tzl_vec2 result;
    tzl_vec2_sub(a, b, result);
    tzl_vec2 expected = {-2.0f, -2.0f};

    return tzl_vec2_equal(result, expected);
}

bool it_scalar_mul_vec2(void)
{
    tzl_vec2 a = {1.0f, 2.0f};
    tzl_vec2 b = {3.0f, 4.0f};

    tzl_vec2 result;
    tzl_vec2_scalar_mul(a, b, result);
    tzl_vec2 expected = {3.0f, 8.0f};

    return tzl_vec2_equal(result, expected);
}

bool it_scalar_div_vec2(void)
{
    tzl_vec2 a = {4.0f, 8.0f};
    tzl_vec2 b = {2.0f, 4.0f};

    tzl_vec2 result;
    tzl_vec2_scalar_div(a, b, result);
    tzl_vec2 expected = {2.0f, 2.0f};

    return tzl_vec2_equal(result, expected);
}

bool it_calculates_vec2_length_and_length_squared(void)
{
    tzl_vec2 v = {3.0f, 4.0f};
    tzl_f32 length;
    tzl_f32 length_squared;
    tzl_vec2_length(v, &length);
    tzl_vec2_length_squared(v, &length_squared);

    return tzl_f32_equal(length, 5.0f) && tzl_f32_equal(length_squared, 25.0f); // 3-4-5 triangle
}

bool it_normalizes_vec2(void)
{
    tzl_vec2 v = {3.0f, 4.0f};
    tzl_vec2 result;
    tzl_vec2_normalize(v, result);

    tzl_f32 length = 0.0f;
    tzl_vec2_length(result, &length);

    return tzl_f32_equal(length, 1.0f); // normalized vector
}

bool it_dots_vec2(void)
{
    tzl_vec2 a = {1.0f, 2.0f};
    tzl_vec2 b = {3.0f, 4.0f};

    tzl_f32 result;
    tzl_vec2_dot(a, b, &result);

    return tzl_f32_equal(result, 11.0f); // 1*3 + 2*4
}

bool it_lerps_vec2(void)
{
    tzl_vec2 a = {1.0f, 2.0f};
    tzl_vec2 b = {3.0f, 4.0f};
    tzl_f32 t = 0.5f;

    tzl_vec2 result;
    tzl_vec2_lerp(a, b, t, result);

    tzl_vec2 expected = {2.0f, 3.0f}; // halfway between a and b
    return tzl_vec2_equal(result, expected);
}

bool it_extends_vec2_to_vec3(void)
{
    tzl_vec2 v = {1.0f, 2.0f};
    tzl_f32 z = 3.0f;

    tzl_vec3 result;
    tzl_vec2_extend(v, z, result);

    tzl_vec3 expected = {1.0f, 2.0f, 3.0f};
    return tzl_vec3_equal(result, expected);
}

bool it_tests_equality_of_vec3(void)
{
    tzl_vec3 a = {1.0f, 2.0f, 3.0f};
    tzl_vec3 b = {1.0f, 2.0f, 3.0f};
    return tzl_vec3_equal(a, b);
}

bool it_normalizes_vec3(void)
{
    tzl_vec3 v = {3.0f, 4.0f, 0.0f};
    tzl_vec3 result;
    tzl_vec3_normalize(v, result);

    tzl_f32 length = 0.0f;
    tzl_vec3_length(result, &length);

    return tzl_f32_equal(length, 1.0f); // normalized vector
}

bool it_tests_equality_of_vec4(void)
{
    tzl_vec4 a = {1.0f, 2.0f, 3.0f, 4.0f};
    tzl_vec4 b = {1.0f, 2.0f, 3.0f, 4.0f};
    return tzl_vec4_equal(a, b);
}

bool it_normalizes_vec4(void)
{
    tzl_vec4 v = {1.0f, 2.0f, 3.0f, 4.0f};
    tzl_vec4 result;
    tzl_vec4_normalize(v, result);

    tzl_f32 length = 0.0f;
    tzl_vec4_length(result, &length);

    return tzl_f32_equal(length, 1.0f); // normalized vector
}

bool run_tzl_tests(void)
{
    printf("Running TZL tests...\n");
    bool has_passed = true;

    run_test(it_tests_equality_of_f32, has_passed);
    run_test(it_lerps_f32, has_passed);

    run_test(it_tests_equality_of_vec2, has_passed);
    run_test(it_adds_vec2_to_vec2, has_passed);
    run_test(it_subs_vec2_from_vec2, has_passed);
    run_test(it_scalar_mul_vec2, has_passed);
    run_test(it_scalar_div_vec2, has_passed);
    run_test(it_calculates_vec2_length_and_length_squared, has_passed);
    run_test(it_normalizes_vec2, has_passed);
    run_test(it_dots_vec2, has_passed);
    run_test(it_lerps_vec2, has_passed);
    run_test(it_extends_vec2_to_vec3, has_passed);

    run_test(it_tests_equality_of_vec3, has_passed);
    run_test(it_normalizes_vec3, has_passed);

    run_test(it_tests_equality_of_vec4, has_passed);
    run_test(it_normalizes_vec4, has_passed);

    if (!has_passed)
    {
        printf("\n\x1b[31mSome tests failed!\x1b[0m\n");
    }

    return has_passed;
}

int main()
{
    return run_tzl_tests() ? 0 : 1;
}